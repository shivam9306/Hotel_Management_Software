#include<stdio.h>
#include<string.h>
void menu(void);
void Add_details(void);
void display(void);
void clear(void);
void Show_recent(void);
void update(void);
void delete_specifically(void);
void generate_bill(void);
struct customer{
    char fname[20];
    char lname[20];
    char address[20];
    int phone_no;
    float amount;
};
int main(){
    while(1){
    menu();
    }
    return 0;
}

    void menu(){
        int num;
        printf("\n------ Welcome to Hotel Managemnet Software ------\n");
        printf("Type 1 to add customer details \n");
        printf("Type 2 to show customer details\n");
        printf("Type 3 to update specific detail\n");
        printf("Type 4 to Display only recently added customer details\n");
        printf("Type 5 to delete specific customer details\n");
        printf("Type 6 to clear all privously recorded customer details\n");
        printf("Type 7 to generate total bill\n");
        printf("Type 8 to Exit the Software\n");
        scanf("%d",&num);
        if(num==1){
            Add_details();
        }
        else if(num==2){
            display();
        }
        else if(num==3){
            update();
        }else if(num==4){
            Show_recent();
        }
        else if(num==5){
            delete_specifically();
        }
        else if(num==6){
            clear();
        }else if(num==7){
            generate_bill();
        }
        else if(num==8){
            printf("\n--Successfully Exited--\n");
            printf("\n---Thanks for using our software---\n");
        }else{
            printf("Invalid input Please! try again\n");
        }
}
void Add_details(){
        struct customer a;
        printf("Enter the customer details \n");
        printf("Enter first name\n");
        scanf("%s",a.fname);
        printf("Enter last name\n");
        scanf("%s",a.lname);
        printf("Enter address \n");
        scanf("%s",a.address);
        printf("Enter phone number\n");
        scanf("%d",&a.phone_no);
        printf("Enter the amount\n");
        scanf("%f",&a.amount);

        FILE *ptr;
        ptr=fopen("HMS.bin","ab");
        if(ptr == NULL) {
            printf("Error opening file.\n");
            return;
        }
        fwrite(&a,sizeof(struct customer),1,ptr);
        fclose(ptr);
        printf("customer details added successfully\n");
 }   

    void display(){
    struct customer a;
     FILE *ptr;
     ptr=fopen("HMS.bin","rb");

     if(ptr == NULL) {
        printf("No customer data found.\n");
        return;
    }

     while(fread(&a,sizeof(struct customer),1,ptr)){
    printf("\n---The cutomer details are---\n");
    printf("The first name is %s\n",a.fname);
    printf("The last name is %s\n",a.lname);
    printf("The address  is %s\n",a.address);
    printf("The phone number is %d\n",a.phone_no);
    printf("The amount  is %f\n",a.amount);
    
     }
     fclose(ptr);
}

void clear(){
    struct customer a;
    FILE *ptr = fopen("HMS.bin", "wb");  // "wb" truncates file to 0 bytes
if(ptr != NULL){
    fclose(ptr);
    printf("All customer records have been deleted.\n");
}
}

void Show_recent(){
    struct customer a;
    FILE *ptr;
    ptr=fopen("HMS.bin","rb");
    if (fseek(ptr, -(long)sizeof(struct customer), SEEK_END) != 0){
        printf("Error seeking to last record.\n");
        fclose(ptr);
        return;
    }
    if (fread(&a, sizeof(struct customer), 1, ptr) == 1) {
        printf("\n--- Most Recently Added Customer Details ---\n");
        printf("First Name: %s\n", a.fname);
        printf("Last Name: %s\n", a.lname);
        printf("Address: %s\n", a.address);
        printf("Phone Number: %d\n", a.phone_no);
        printf("Charges: %.2f\n", a.amount);
    } else {
        printf("Error reading customer data.\n");
    }
    fclose(ptr);
}

void update(){
    struct customer a;
    int found = 0;
    int phone;
    FILE *ptr = fopen("HMS.bin", "rb+"); // read + write mode

    if(ptr == NULL){
        printf("Error opening file.\n");
        return;
    }

    printf("Enter the phone number of the customer to update: ");
    scanf("%d", &phone);

    while(fread(&a, sizeof(struct customer), 1, ptr)){
        if(a.phone_no == phone){
            found = 1;

            printf("\n--- Current Details ---\n");
            printf("First Name: %s\n", a.fname);
            printf("Last Name: %s\n", a.lname);
            printf("Address: %s\n", a.address);
            printf("Phone Number: %d\n", a.phone_no);
            printf("Charges: %.2f\n", a.amount);

            printf("\nEnter new details:\n");
            printf("Enter first name: ");
            scanf("%s", a.fname);
            printf("Enter last name: ");
            scanf("%s", a.lname);
            printf("Enter address: ");
            scanf("%s", a.address);
            printf("Enter phone number: ");
            scanf("%d", &a.phone_no);
            printf("Enter charges: ");
            scanf("%f", &a.amount);

            // Move file pointer back to beginning of this record
            fseek(ptr, -(long)sizeof(struct customer), SEEK_CUR);
            fwrite(&a, sizeof(struct customer), 1, ptr);
            printf("Customer details updated successfully.\n");
            break;
        }
    }

    if(!found){
        printf("Customer with phone number %d not found.\n", phone);
    }

    fclose(ptr);
}

void delete_specifically(){
    struct customer a;
    int phone;
    int found = 0;

    FILE *ptr = fopen("HMS.bin", "rb");
    FILE *temp = fopen("temp.bin", "wb");

    if(ptr == NULL || temp == NULL){
        printf("Error opening file.\n");
        return;
    }

    printf("Enter the phone number of the customer to delete: ");
    scanf("%d", &phone);

    while(fread(&a, sizeof(struct customer), 1, ptr) == 1){
        if(a.phone_no == phone){
            found = 1;
            printf("Customer with phone number %d will be deleted.\n", phone);
            continue;  // skip writing this record
        }
        fwrite(&a, sizeof(struct customer), 1, temp);  // write other records
    }

    fclose(ptr);
    fclose(temp);

    if(found){
        remove("HMS.bin");             // delete original file
        rename("temp.bin", "HMS.bin"); // rename temp file to original name
        printf("Customer record deleted successfully.\n");
    } else {
        remove("temp.bin"); // no record matched; discard temp file
        printf("Customer with phone number %d not found.\n", phone);
    }
}

void generate_bill(){
    struct customer a;
    float gst_rate, service_charge_rate;
    float total_all = 0;

    FILE *ptr = fopen("HMS.bin", "rb");
    FILE *receipt = fopen("bill.txt", "w");

    if(ptr == NULL || receipt == NULL){
        printf("Error opening file(s).\n");
        return;
    }

    // Get rates from user
    printf("Enter GST rate (e.g., enter 18 for 18%%): ");
    scanf("%f", &gst_rate);
    gst_rate /= 100.0;

    printf("Enter Service Charge rate (e.g., 5 for 5%%): ");
    scanf("%f", &service_charge_rate);
    service_charge_rate /= 100.0;

    fprintf(receipt, "--------- HOTEL BILL RECEIPT ---------\n");

    while(fread(&a, sizeof(struct customer), 1, ptr) == 1){
        float gst = a.amount * gst_rate;
        float service = a.amount * service_charge_rate;
        float total = a.amount + gst + service;
        total_all += total;

        // Display on console
        printf("\nCustomer: %s %s\n", a.fname, a.lname);
        printf("Room Charges: %.2f\n", a.amount);
        printf("GST @%.2f%%: %.2f\n", gst_rate * 100, gst);
        printf("Service Charge @%.2f%%: %.2f\n", service_charge_rate * 100, service);
        printf("Total Bill: %.2f\n", total);

        // Write to file
        fprintf(receipt, "\nCustomer: %s %s\n", a.fname, a.lname);
        fprintf(receipt, "Room Charges: %.2f\n", a.amount);
        fprintf(receipt, "GST @%.2f%%: %.2f\n", gst_rate * 100, gst);
        fprintf(receipt, "Service Charge @%.2f%%: %.2f\n", service_charge_rate * 100, service);
        fprintf(receipt, "Total Bill: %.2f\n", total);
    }

    // Final total
    printf("\nTOTAL AMOUNT FOR ALL CUSTOMERS: %.2f\n", total_all);
    fprintf(receipt, "\n--------------------------------------\n");
    fprintf(receipt, "TOTAL AMOUNT FOR ALL CUSTOMERS: %.2f\n", total_all);
    fprintf(receipt, "--------- END OF RECEIPT ------------\n");

    fclose(ptr);
    fclose(receipt);

    printf("\nFull bill saved to 'bill.txt'\n");
}


