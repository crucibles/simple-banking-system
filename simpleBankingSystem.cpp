/*
This is a simple banking system implemented by...
- Torres, Sumandang, and Alvaro.

The simple banking system can:

1. create new account with account types:
	- savings
	- current
2. read account information;
3. update account information;
4. delete account;
5. withdraw/deposit/transfer money from/to accounts, and;
6. increase the deposited money base on the interest rate depending on the account type. (Savings or Current)

*/
#include<stdio.h>
#include<windows.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>
#include<string.h>
#include<errno.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"
#define BUFFER_SIZE 100
#define SAVINGS_INTEREST_RATE 0.025
#define CURRENT_INTEREST_RATE 0.015

struct Date{
	int month, day, year;
};

struct Transaction{
	int transId;
	int transType;
	struct Date transDate;
	int accNumber;
	float amount;
	int toAccount;
};

struct TransactionLog{
	struct Transaction transaction;
	struct TransactionLog *next;
};

struct Account{
	char firstName[100];
	char middleName[100];
	char lastName[100];
	char pin[6];
	int accNumber;
	struct Date birthDate;
	float balance;
	int accType; // 0 = savings, 1 = current
	struct Date startDate;
};

int newAccNumber();
char *newPin();
int confirm();
int count();
void start();
void menu();

//GUI functions
void gotoxy(int x, int y);
void rect();
void rect(int,int,int,int);
void hline(int,int,int);
void vline(int,int,int);

//General helper functions
int getUserInput();
char* fgetsW(char* buffer, int bufferLength, FILE *fp);

//Account functions
void createAccount();
void closeAccount();
void displayAcc(int accNumber);
Account getAccount();
Account searchAccount(int accNumber);
void updateAccount();
//Account helper functions
void postAccount(Account);
void postAccountUpdate(struct Account currAccount);
void updateMenu(Account currAccount);
void updateProcess(int choice, struct Account currAccount);

//Transaction Functions
struct TransactionLog *getTransactionsOf(int accountNumber);
void deposit();
void depositAmountTo(float amount, int accNumber);
void transactions();
void transfer();
void viewTransactionLogs();
void withdraw();
//Transaction helper functions
void askAccNum();
float calculateTransactionAmount(int transType, float amount);
int checkBalance(float amount,int accountNum);
Account getAccount(int accountNum);
Date getDate();
float getInterest(Account currAccount);
int getLatestTransactionId();
struct TransactionLog *getTransactions();
float getTotal(int transType,int accNumber);
float getYearBalance(struct TransactionLog *transLog, Date startDate);
void postTransaction(int accNumber);
void postTransfer(Transaction newTransaction);
void process(Transaction newTransaction);
void updateTransId(int latestTransId);
int validate();

//Security functions
bool dateChecker(int,int,int);
bool intChecker();

int transactionIdCounter=getLatestTransactionId();
int tellerPin=1234;

main(){

	int i = 0,x = 1;
	
	gotoxy(15,15);
	system("color 2");
	printf("Loading: ");
	
	for(i=23;i<=56;i++){
		gotoxy(i,15);
		Sleep(30);
        printf("\xB2");
        gotoxy(59,15);
        printf(" [%d %]", x);
        x+=3;
    }
    printf("\n");
    gotoxy(25,19);
    system("pause");
    system("cls");
	start();
	
	return 0;
}


/*
	@author Cedric Alvaro

	@description checks date if it is valid
	@params are the ones to be checked if it is valid in int format so Dec 12, 2018 is 12/12/2018 MM/DD/YY.
	
*/
bool dateChecker(int month, int day, int year){
	int mm = month, dd = day, yy = year;

	
    if(yy>=1900 && yy<=9999){
    	
        if(mm>=1 && mm<=12){
        	
            if((dd>=1 && dd<=31) && (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12)){
            	return true;
			} else if((dd>=1 && dd<=30) && (mm==4 || mm==6 || mm==9 || mm==11)){
				return true;
			} else if((dd>=1 && dd<=28) && (mm==2)){
				return true;
			} else if(dd==29 && mm==2 && (yy%400==0 ||(yy%4==0 && yy%100!=0))){
				return true;
			} else{
				printf("	Date invalid. \n");
				return false;	
			}
                
        } else{
            printf("	Month is not valid.\n");
            return false;
        }
        
    } else{
        printf("	Year is not valid.\n");
        return false;
    }

}

/*
	@author Cedric Alvaro

	@description prints the transaction menu, and a list of choices. It will call those functions then.
	
*/
void transactions(){
	char choice;
	
	system("color 9");
	printf("\n\n\t\t\tSIMPLE BANKING MANAGEMENT SYSTEM");
    printf("\n\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 WELCOME TO TRANSACTIONS MENU \xB2\xB2\xB2\xB2\xB2\xB2\xB2");
    printf("\n\n\t\t"
	"1.Withdraw\n\t\t"
	"2.Deposit\n\t\t"
	"3.Transfer Money\n\t\t"
	"0.Go back to Main Menu\n\n\n\n\n\n\n\n\t\t"
	"Enter your choice:");
	
	rect(4, 4, 77, 15);
	
	gotoxy(35,18);
	scanf("%d",&choice);
    system("cls");

    switch(choice) {
		case 1:
	        withdraw();
	        break;
	    case 2:
	    	deposit();
	    	break;
	    case 3:
	    	transfer();
	    	break;
	    case 0:
	       	menu();
        break;     
    }
    menu();
}


/*
	@author Cedric Alvaro

	@description It will draw a rectangle
	@params
		x1 is the starting of the horizontal lines
		x2 the end of the horizontal lines
		y1 the starting of vertical lines
		y2 the ending of vertical lines
*/
void rect(int x1,int y1,int x2, int y2)
{
  /*printing corners*/
  gotoxy(x2,y1);
  printf("%c",191);
  gotoxy(x1,y2);
  printf("%c",192);
  gotoxy(x2,y2);
  printf("%c",217);
  gotoxy(x1,y1);
  printf("%c",218);
  // printing lines
  hline(x1+1,x2-1,y1);
  hline(x1+1,x2-1,y2);
  vline(y1+1,y2-1,x1);
  vline(y1+1,y2-1,x2);
}


/*
	@author Cedric Alvaro

	@description It will draw a horizontal line
	@params
		x1 is the starting of the horizontal line
		x2 is the end of the horizontal line
		y the position base on y-axis where to put that horizontal line
	
*/
void hline(int x1,int x2,int y)
{
  int i;
  for(i=x1;i<=x2;i++)
  {
    gotoxy(i,y);
    printf("%c",196);
  }
}


/*
	@author Cedric Alvaro

	@description It will draw a vertical line
	@params
		y1 is the starting of the vertical line
		y2 is the end of the vertical line
		x is the position base on x-axis where to put that vertical line
	
*/
void vline(int y1,int y2,int x)
{
  int i;
  for(i=y1;i<=y2;i++)
  {
    gotoxy(x,i);
    printf("%c",179);
  }
}


/*
	@author Cedric Alvaro

	@description It will print a series of actions for the teller/user to chhoose from.
	It will then call those functions, for the processes.
	
*/
void menu(){
	int choice;
    system("cls");
    system("color 9");
    
    
    printf("\n\n\t\t\tSIMPLE BANKING MANAGEMENT SYSTEM");
    printf("\n\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 WELCOME TO THE MAIN MENU \xB2\xB2\xB2\xB2\xB2\xB2\xB2");
    printf("\n\n\t\t"
	"1.Create new account\n\t\t"
	"2.Update information of existing account\n\t\t"
	"3.For transactions\n\t\t"
	"4.Check the details of existing account\n\t\t"
	"5.Close an existing account\n\t\t"
	"6.View Account Transaction Logs\n\t\t"
	"0.Exit\n\n\n\n\n\t\t"
	"Enter your choice:");
	rect(4, 4, 77, 15);
	
	gotoxy(35,18);
	scanf("%d",&choice);
    system("cls");

    switch(choice) {
		case 1:
	        createAccount();
	        break;
	    case 2:
	    	updateAccount();
	    	break;
	    case 3:
	    	transactions();
	    	break;
	    case 4:
	       	askAccNum();
	       	break;
	    case 5:
	    	closeAccount();
	     	break;
	    case 6:
	    	viewTransactionLogs();
	       	break;
	    case 0:
	       	start();
        	break;     
    }
    menu();	
}


/*
	@author Cedric Alvaro
	@modified Sumandang, AJ Ruth (12/30/2018)

	@description It will ask the teller to input the manager password for role-locked actions such as delete, etc.
	@returns an integer that will determin if the inputted password is correct.
		1 : true
		0: false
	
*/
int confirm(){
	
	char password[] = "manager";
	char *pass = new char[1024];
    int i=0, tries=3;
    
    char input;
	do{
		if (!(input == 127 || input == 8)) {
			printf("*");
			pass[i] = input;
			i++;
		} else if (i!=0){
			printf("\b \b");
			i--;
		}
		input = getch();
	} while(input != '\r');

    
    while(tries > 0){
	    system("cls");
		printf("\n\n\t\t\tSIMPLE BANKING MANAGEMENT SYSTEM");
		system("color 6");
		rect(5, 7, 75, 15);
		gotoxy(20,10);
		printf("\tPassword to confirm this action: ");
		scanf("%s",pass);

	
		if (strcmp(pass,password)==0){
    	gotoxy(24,18);
    	
		printf("Processing ");
		for(i=0;i<=6;i++){
			Sleep(200);
            printf(".");
        }
        printf("\n");
        
        return 1;
	    } else {
	    	tries--;
			gotoxy(25,18);
			printf("Password is wrong! Remaining tries: %d", tries);
		
			for(i=0;i<=6;i++){
				Sleep(200);
	            printf(".");
	        }
		}
	}
	
	return 0;
}


/*
	@author Cedric Alvaro

	@description It will ask for the account number of the user to be deleted. It will auto withdraw all his/her balance, then closes the account.
	Deleting the info from the accounts.in.
	
*/
void closeAccount(){
	int remAcc;
	int counter = count();
	Account searchedAcc;
	char choice;
	char blank[100];
	
	// call helper function of withdraw();
	// withdraw();

	FILE *fr,*newrec;
    fr=fopen("account.in","r");
    newrec=fopen("new_account.in","w");
    
    
    printf("Enter the account no. of the user you want to delete: ");
    scanf("%d", &remAcc);
    displayAcc(remAcc);
    fflush(stdin);
    printf("\n\nAre you sure you want to delete this account?\nYou cannot undo this action (Y/N): ");
    scanf("%c", &choice);
    
    if(choice == 'y' || choice=='Y'){
    	
    	if (confirm()){
    		while(counter > 1){
				fgets(searchedAcc.firstName, 100, fr);
				fgets(searchedAcc.middleName, 100, fr);
				fgets(searchedAcc.lastName, 100, fr);
				fscanf(fr, "%d/%d/%d", &searchedAcc.birthDate.month, &searchedAcc.birthDate.day, &searchedAcc.birthDate.year);
				fscanf(fr, "%f", &searchedAcc.balance);
				fscanf(fr, "%d", &searchedAcc.accType);
				fscanf(fr, "%d/%d/%d", &searchedAcc.startDate.month, &searchedAcc.startDate.day, &searchedAcc.startDate.year);
				fscanf(fr, "%d", &searchedAcc.accNumber);
				fscanf(fr, "%d", &searchedAcc.pin);
				// skip two lines
				fgets(blank, 100, fr);
				fgets(blank, 100, fr);
				
				if(remAcc != searchedAcc.accNumber){
					fprintf(
						newrec,
						"%s%s%s%d/%d/%d\n%.2f\n%d\n%d/%d/%d\n%d\n%d\n\n",
						
						searchedAcc.firstName, 
						searchedAcc.middleName, 
						searchedAcc.lastName, 
						searchedAcc.birthDate.month, 
						searchedAcc.birthDate.day, 
						searchedAcc.birthDate.year, 
						searchedAcc.balance, 
						searchedAcc.accType, 
						searchedAcc.startDate.month, 
						searchedAcc.startDate.day, 
						searchedAcc.startDate.year, 
						searchedAcc.accNumber, 
						searchedAcc.pin 
					);
	
				}
				counter--;
			}
			    fclose(fr);
				fclose(newrec);
				if (remove("account.in") != 0 ){
					printf("\nSystem error in updating the new list of accounts.\n");
					system("pause");
				}
			    if (rename("new_account.in","account.in") == -1){
			    	printf("\nSystem error in renaming the new list of accounts.\n");
			    	system("pause");
				}
			
				system("pause");
			printf("\n\n\t\tClosing account succesful!\n\t\t");
		}

	} else {
		printf("\n\nGoing back to the main menu!");
		system("pause");
		fclose(fr);
		fclose(newrec);
		return;
	}
    

}


/*
	@author Cedric Alvaro

	@description Tells the teller to input his/her password first before he/she can use the system.
	
*/
void start(){
	char pass[1024];
	char password[]="teller";
    int i=0, x=0, key;
    
	printf("\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 SIMPLE BANKING MANAGEMENT SYSTEM \xB2\xB2\xB2\xB2\xB2\xB2\xB2");
	system("color 6");
	
	rect(5, 7, 75, 15);
	gotoxy(20,10);
	printf("\tEnter the password to login: ");
	
	char input = getch();
	while (input != '\r')
	{
		if (!(input == 127 || input == 8)) {
			printf("*");
			pass[i] = input;
			i++;
		} else if (i!=0){
			printf("\b \b");
			i--;
		}
		input = getch();
	}
	pass[i]='\0';
	
    if (strcmp(pass,password)==0){
    	gotoxy(24,18);
    	
		printf("Preparing the system for you ");
		for(i=0;i<=6;i++){
			Sleep(50);
            printf(".");
        }
        
		system("cls");
		menu();
    } else {
    	gotoxy(25,15);
		printf("Password is wrong! Input again.\n\n");
		gotoxy(25,18);
		system("pause");
		system("cls");
		start();
	}
    
}


/*
	@author Cedric Alvaro

	@description It will move the cursor to that location
	@param x is the x-axis (horizontal)
	@param y is the y-axis (vertical)
	
*/
void gotoxy(int x, int y){
	static HANDLE h = NULL;  
	if(!h){
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	COORD c = { x, y };
	SetConsoleCursorPosition(h,c);
}


/*
	@author Cedric Alvaro

	@params accNumber this will be used and be searched in the account.in file
	
	@description It will find the line number of accNumber to be searched, 
	since accNumber is the unique and unchangeable attribute in each bank account.
	
	@returns int that is the line number of that accNumber in accounts.in file
	
*/
int getAccNumLine(int accNumber){
	int line = 0, an;
	char blank[100];	
	FILE *fr;
	fr = fopen("account.in", "r");
	
	while(fgetsW(blank, 100, fr) != NULL){
		if(line % 10 == 7 && (atoi(blank) == accNumber)){
			fclose(fr);
			return line;
		}
		line++;
	}
	
	fclose(fr);
	return 0;
}


/*
	An fgtets wrapper function.
	It modifies the fgets function by removing the /n at the end (if it exists).
	Takes the same parameters as fgets.
	@param buffer 		the string to put edits
	@param bufferLength	the length of buffer
	@param file			file pointer
	
	@return the resulting string if successful; return null pointer constant, 0, if unsuccessful.
	
	@author Sumandang, AJ Ruth H. (12/28/2018)
*/
char *fgetsW(char *buffer, int bufferLength, FILE *fp){
	if (fgets(buffer, bufferLength, fp) != 0){
        int len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n'){
        	buffer[len-1] = '\0';
		}
        return buffer;
    }
    return 0;
}


/*
	@author Cedric Alvaro

	@description It will search an account element from the list of accounts in accounts.in
	@params accNumber is the account to be searched, since it is the unique attribute
	@return a struct Account instance
	
*/
Account searchAccount(int accNumber){
	Account searchedAcc;
	searchedAcc.accNumber = 0;
	FILE *fr;
	fr = fopen("account.in", "r");
	int line, i = 0;
	char blank[100];

	line = getAccNumLine(accNumber) - 7;
	
	if(line >= 0){
		while(i != line){
			fgets(blank, 100, fr);
			i++;	
		}
		
		fgets(searchedAcc.firstName, 100, fr);
		fgets(searchedAcc.middleName, 100, fr);
		fgets(searchedAcc.lastName, 100, fr);
		fscanf(fr, "%d/%d/%d", &searchedAcc.birthDate.month, &searchedAcc.birthDate.day, &searchedAcc.birthDate.year);
		fscanf(fr, "%f", &searchedAcc.balance);
		fscanf(fr, "%d", &searchedAcc.accType);
		fscanf(fr, "%d/%d/%d", &searchedAcc.startDate.month, &searchedAcc.startDate.day, &searchedAcc.startDate.year);
		fscanf(fr, "%d", &searchedAcc.accNumber);
		fscanf(fr, "%s", searchedAcc.pin);
	}
	fclose(fr);
	return searchedAcc;
}

/*
	@author Cedric Alvaro

	@description asks input accNumber to be shown as account
	
*/
void askAccNum(){
	int accNumber;
	
	printf("Enter the account number: ");
	scanf("%d", &accNumber);
	
	displayAcc(accNumber);
}

/*
	@author Cedric Alvaro

	@description It will ask the teller to input the account number of the user, then searches it from the accounts.in
	It will then print the details of that account.
	
*/
void displayAcc(int accNumber){
	Account user;
	
	user = searchAccount(accNumber);
	
	if(user.accNumber != 0){
		printf(
			"Here are the account details:\n\n"
			
			"First name:		%s"
			"Middle name:		%s"
			"Last name:		%s"
			"BirthDate (MM/DD/YY):	%d/%d/%d\n"
			"Balance:		%.2f\n"
			"Account Type:		%s\n"
			"Account Created (MM/DD/YY): %d/%d/%d\n"
			"Account Number:		%d\n"
			"Account Pin:		%s\n\n",
			
			user.firstName, 
			user.middleName, 
			user.lastName, 
			user.birthDate.month, user.birthDate.day, user.birthDate.year, 
			user.balance, 
			user.accType==0? "savings": "current", 
			user.startDate.month, user.startDate.day, user.startDate.year, 
			user.accNumber, 
			user.pin
		);
	}
	
	system("pause");
}


/*
	@author Cedric Alvaro

	@description It will create an account for a user then writes its information to a file named "account.in".
	
	The order of data is respectively:
	firstName : string
	middleName : string
	lastName : string
	birthDate	: struct Date
	balance	: float
	accType	: int
	startDate : struct Date
	accNumber	:	int
	pin	:	int
	
*/
void postAccount(Account add){
	FILE *fr;
	fr = fopen("account.in", "ab");
	
	fprintf(
	fr,
	"%s\n%s\n%s\n%d/%d/%d\n%.2f\n%d\n%d/%d/%d\n%d\n%s\n\n",
	add.firstName, add.middleName, add.lastName, add.birthDate.month, add.birthDate.day, add.birthDate.year, add.balance, add.accType, add.startDate.month, add.startDate.day, add.startDate.year, add.accNumber, add.pin 
	);
	
	fclose(fr);
	system("cls");
	printf("\n\nThis is your information, Please edit later if you want.\n\n\n");
	displayAcc(add.accNumber);
}

/*
	@author Cedric Alvaro

	@description asks for input to a new account
	@return a struct Account instance
	
*/
void createAccount(){
	Account add;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int counter = 0;
	char *input = new char[1024];
	char c;
	
	printf(
	"\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2"
	" CREATION OF ACCOUNT "
	"\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n\n"
	);
	
	char *interact[6] = {
	"	Enter first name: ", 
	"	Enter middle name: ", 
	"	Enter last name: ", 
	"	Enter birthdate in MM/DD/YYYY format: ", 
	"	Enter starting amount: ",
	"	Choose what account type [0 - savings, 1 - current]: "
	};
	
	
	
	while(counter!= 6){
		printf("%s",interact[counter]);
		switch(counter){
			case 0:
				fflush(stdin);
				fgetsW(add.firstName, 100, stdin);
				break;
			case 1:
				fgetsW(add.middleName, 100, stdin);
				break;
			case 2:
				fgetsW(add.lastName, 100, stdin);
				break;
			case 3:
				scanf("%d/%d/%d",&add.birthDate.month, &add.birthDate.day, &add.birthDate.year);
					while(!dateChecker(add.birthDate.month, add.birthDate.day, add.birthDate.year)){
						printf("	Please input a valid date: ");
						scanf("%d/%d/%d",&add.birthDate.month, &add.birthDate.day, &add.birthDate.year);
					}
				break;
			case 4:
				scanf("%f", &add.balance);
				break;
			case 5:
				scanf("%d", &add.accType);
				break;
		}
			
    	counter++;
	}
	
	char localDate[9];
	_strdate(localDate);
	add.startDate.month = atoi(localDate);
	add.startDate.day = atoi(&localDate[3]);
	add.startDate.year = atoi(&localDate[6]);
	
	add.accNumber = newAccNumber();
	strcpy(add.pin, newPin());
	
    postAccount(add);
    depositAmountTo(add.balance, add.accNumber);
    system("cls");
}

/*
	@author Cedric Alvaro

	@description It will find the next account number based on the last used account number then increments it by one.
	@returns an integer that is the account number of a newly created account.
	
*/
int newAccNumber(){
	FILE *fr;
	fr = fopen("account.in", "r");
	char *line = new char[1024];
	int counter = 0;
	int accNumber = 0;
	
	do{
		if((counter - 7)%10 == 0 || counter-7 == 0){
			fscanf(fr, "%d", &accNumber);
		}
		line = fgets(line, 100, fr);
		counter ++;
	} while(line != NULL);
	
	accNumber++;
	
	fclose(fr);
	return accNumber;
}

/*
	@author Cedric Alvaro

	@description It will generate a new pin based on the last pin used just incrementing it by one.
	@returns an integer that will be the pin of a newly created account.
	
*/
char *newPin(){
	FILE *fr;
	fr = fopen("account.in", "r");
	char *line = new char[1024];
	char *pin = new char[5];
	int counter = 0;
	
	do{
		line = fgets(line, 100, fr);
		counter ++;
	}while(line != NULL);
	
	counter /= 10;
	counter++;
	
	fclose(fr);
	itoa(counter, pin, 10);
	system("pause");
	return pin;
}

/*
	@author Cedric Alvaro

	@description It will count how many users are there in the banking system
	@returns an integer that is the account number of a new created account.
	
*/
int count(){
	FILE *fr;
	fr = fopen("account.in", "r");
	char *line = new char[1024];
	int counter = 0;
	
	do{
		line = fgets(line, 100, fr);
		counter ++;
	}while(line != NULL);
	
	counter /= 10;
	counter++;
	
	fclose(fr);
	return counter;
}


/**
	Updates an existing customer's account.
	Contains all the function necessary for updating such as menu, update process, and looping.
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
void updateAccount(){
	int choice;
	Account currAccount;
	
	currAccount = getAccount();
	
	if(currAccount.accNumber == 0){
		return;
	} else {
		do{
			updateMenu(currAccount);
			/* TODO (#2#): getUserINput still has bugs (chooses 1); fixed or replace with a better security function */
			choice = getUserInput();
			updateProcess(choice, currAccount);
			currAccount = searchAccount(currAccount.accNumber);
			system("cls");
		} while(choice != 0);
	}
}

/**
	Contains the update-account menu.
	Displays the options the user could choose from.
	
	@author Sumandang, AJ Ruth (12/28/2018)	
*/
void updateMenu(struct Account currAccount){
	printf("Input number you want to update...\n");
	printf("[1] First name: 	%s", currAccount.firstName);
	printf("[2] Middle name: 	%s", currAccount.middleName);
	printf("[3] Last name: 		%s", currAccount.lastName);
	printf("[4] Birthdate(M/D/Y): 	%d/%d/%d\n", currAccount.birthDate.month, currAccount.birthDate.day, currAccount.birthDate.year);
	printf("[5] PIN: 		%s\n", currAccount.pin); 
	printf("[0] No changes. Exit to main menu.\n", currAccount.pin);
	printf("Input number: ");
}

/**
	Contains the processes necessary for updating current account.
	@param choice user choice; (1) first name, (2) middle name, (3) last name, (4) birthdate, (5) PIN
	@param contains the current account to be updated
	
	@author Sumandang, AJ Ruth H. (12/28/2018)
*/
void updateProcess(int choice, struct Account currAccount){
	char name[100];
	char pin[6];
	struct Date birthdate;
	
	switch(choice){
		case 1:
			printf("Enter First Name (change): ");
			fgetsW(name, BUFFER_SIZE, stdin);
			strcpy(currAccount.firstName, name);
			strcat(currAccount.firstName, "\n");
			postAccountUpdate(currAccount);
			break;
		case 2:
			printf("Enter Middle Name (change): ");
			fgetsW(name, BUFFER_SIZE, stdin);
			strcpy(currAccount.middleName, name);
			strcat(currAccount.middleName, "\n");
			postAccountUpdate(currAccount);
			break;
		case 3:
			printf("Enter Last Name (change): ");
			fgetsW(name, BUFFER_SIZE, stdin);
			strcpy(currAccount.lastName, name);
			strcat(currAccount.lastName, "\n");
			postAccountUpdate(currAccount);
			break;
		case 4:
			printf("Enter Birthdate(change)(MM/DD/YYYY format): ");
			scanf("%d/%d/%d", &birthdate.month, &birthdate.day, &birthdate.year);
			currAccount.birthDate.month = birthdate.month;
			currAccount.birthDate.day = birthdate.day;
			currAccount.birthDate.year = birthdate.year;
			postAccountUpdate(currAccount);
			break;
		case 5:
			printf("Enter PIN (change): ");
			fgetsW(pin, 6, stdin);
			pin[6] = '\0';
			strcpy(currAccount.pin, pin);
			postAccountUpdate(currAccount);
			break;
		case 0:
			break;
		default:
			printf("Invalid input! Please input again...\n");
			system("pause");
			break;
	}
}

/**
	Updates the account file (account.in).
	@param currAccount the account to be updated.
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
void postAccountUpdate(struct Account currAccount){
	
	FILE *fr, *fTemp;
	int newLineNum, count = 0;
	char *line = new char[1024];
    char newline[BUFFER_SIZE];
    char tempFileName[] = "replace.in";
    char accountFileName[] = "account.in";
    char lineString[100];
	char temp[50];
    
	newLineNum = getAccNumLine(currAccount.accNumber) - 7;
	
	fr = fopen(accountFileName, "r");
	fTemp = fopen(tempFileName, "w");
	while (fgets(line, BUFFER_SIZE, fr) != NULL){
		// changes the lines of the account with the received currAccount
        if ((count >= newLineNum) && (count - newLineNum < 10)){
        	switch(count - newLineNum){
        		case 0: 
        			fputs(currAccount.firstName, fTemp);
        			break;
        		case 1: 
        			fputs(currAccount.middleName, fTemp);
        			break;
        		case 2: 
        			fputs(currAccount.lastName, fTemp);
        			break;
        		case 3: 
        			sprintf(lineString, "%d", currAccount.birthDate.month);
        			strcat(lineString, "/");
        			sprintf(temp, "%d", currAccount.birthDate.month);
        			strcat(lineString, temp);
        			strcat(lineString, "/");
        			strcpy(temp, "");
        			sprintf(temp, "%d", currAccount.birthDate.year);
        			strcat(lineString, temp);
        			fputs(lineString, fTemp);
        			fputs("\n", fTemp);
        			break;
        		case 4:
        			strcpy(lineString, "");
        			sprintf(lineString, "%f", currAccount.balance);
        			fputs(lineString, fTemp);
        			fputs("\n", fTemp);
        			break;
        		case 8:
        			fputs(currAccount.pin, fTemp);
        			fputs("\n", fTemp);
        			break;
        		default:
        			fputs(line, fTemp);
			}
		} else {
			fputs(line, fTemp);
		}
            
		count++;
    }
    
    fclose(fr);
    fclose(fTemp);
    
    if(remove(accountFileName) != 0){
    	printf("(Err1)Error in editing the file. Please contact developer for help.\n");
    	printf("|%s|\n", strerror(errno));
    	return;
	} 
    if(rename(tempFileName, accountFileName) != 0){
    	printf("(Err2) Error in editing the file. Please contact developer for help.\n");
    	return;
	}
}

/**
	Retrieves the account number from the user.
	If invalid input or non-existing account, prompts the user if they want to exit or input again.
*/
Account getAccount(){
	int choice;
	int accountNumber;
	Account currAccount;
	do{
		choice  = 0;
		printf("Please input account number to update: ");
		scanf("%d", &accountNumber);
		currAccount = searchAccount(accountNumber);
	
		if(currAccount.accNumber == 0){
			printf("Invalid input or the account does not exist. Do you want to input again or exit to main menu?\n");
			printf("[1] Input Again");
			printf("[0] Exit to Main Menu");
		}
	} while(choice != 0);
	
	return currAccount;
}

/**
	Retrieved user's input.
	@return the user choice input
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
int getUserInput(){
	int choice;
	do{   
    	scanf(" %d", &choice);
    	while(getchar() != '\n'); 
	} while (choice < 0);
	
	return choice;
}

/**
	Allows the user to view an existing customer's transaction.
	Contains the function necessary for viewing transaction logs such as getting transaction(in struct form).
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
void viewTransactionLogs(){
	TransactionLog *transactionLog;
	Transaction transaction;
	transactionLog = getTransactions();	
	
	while(transactionLog != NULL){
		transaction = transactionLog -> transaction;
		
		char transType[][20] = {"Withdrawal", "Deposit", "Closing Account"};
		if(transaction.accNumber != 0){
			printf("Transaction ID: 	%d\n", transaction.transId);
			printf("Account Number: 	%d\n", transaction.accNumber);
			printf("Transaction Date: 	%d/%d/%d\n", transaction.transDate.month, transaction.transDate.day, transaction.transDate.year);
			printf("Transaction Type: 	%s\n", transType[transaction.transType]);
			printf("Transaction Amount:	%f\n", transaction.amount);
			(transaction.toAccount != 0)? printf("Transferred to Accnt#:	%d\n\n", transaction.accNumber): printf("\n\n");
		}
		
		transactionLog = transactionLog -> next;
	}
	system("pause");
}

/**
	Retrieves a customer's transaction from the files.
	@return customer transaction logs (in struct Transaction form)
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
struct TransactionLog *getTransactions(){
	int choice = 0, accountNumber;
	struct TransactionLog *transLog = new TransactionLog;
	transLog = NULL;
	
	do{
		choice = 0;
		printf("Input account number to view: ");
		scanf("%d", &accountNumber);
		transLog = getTransactionsOf(accountNumber);
		
		if(transLog == NULL){
			printf("\n\nInvalid input or non-existent account. Do you want to input again?\n");
			printf("[1] Input Again\n");
			printf("[0] Exit to Main Menu\n");
			printf("Input choice: ");
			scanf("%d", &choice);
		} 
	} while(choice != 0);
	
	return transLog;
}


struct TransactionLog *getTransactionsOf(int accountNumber){
	FILE *fr;
	char accountFileName[100];
	
	struct TransactionLog *transLog = new TransactionLog;
	transLog = NULL;
	sprintf(accountFileName, "%d", accountNumber);
	strcat(accountFileName, ".tr");
	
	fr = fopen(accountFileName, "r");
	
	if(fr == NULL){
		printf("FILE IS NULL");
		return NULL;
	}
	
	char *line = new char[1024];
	struct Transaction transaction;
	
	do{
		fscanf(fr, "%d", &transaction.transId);
		fscanf(fr, "%d", &transaction.transType);
		fscanf(fr, "%d", &transaction.accNumber);
		fscanf(fr, "%d/%d/%d", &transaction.transDate.month, &transaction.transDate.day, &transaction.transDate.year);
		fscanf(fr, "%f", &transaction.amount);
		fscanf(fr, "%d", &transaction.toAccount);
		fgets(line, 100, fr);
				
		struct TransactionLog *newLog = new TransactionLog;
		newLog->transaction = transaction;
		newLog->next = transLog;
		transLog = newLog;
	} while(!feof(fr) && line != NULL);
	
	fclose(fr);
	return transLog;
}



/**
	Gets the interest of a given customer account.
	@param the customer account whose interest is to be calculated
	@return the year interest
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
float getInterest(Account accnt){
	/* TODO (#1#): AHJ: delete below if received account is okay. */
	accnt.startDate.day = 27;
	accnt.startDate.month = 12;
	accnt.startDate.year = 2017;
	accnt.accType = 1;
	accnt.balance = 1125;
	accnt.accNumber = 12;
	//AHJ: end here
		
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	Date startDate = accnt.startDate;
	Date currDate;
	currDate.day = tm.tm_mday;
	currDate.month = tm.tm_mon + 1;
	currDate.year = tm.tm_year + 1900;
	
	int yearLapse = currDate.year - startDate.year;
	float interestRate = 0;
	float interest = 0;
	
	//determines whether a year or more had passed by
	if(yearLapse > 0){
		if(currDate.month - startDate.month < 0){
			yearLapse--;
		} else if(currDate.month - startDate.month == 0 && currDate.day - startDate.day < 1){
			yearLapse--;
		}
	} 
	
	//calculates interest if a year or more passed
	if(yearLapse > 0){
		struct TransactionLog *transLog;
		
		if(accnt.accType == 0){
			interestRate = SAVINGS_INTEREST_RATE;
		} else if(accnt.accType == 1){
			interestRate = CURRENT_INTEREST_RATE;	
		} else {
			printf("Unknown account type. Exiting...\n");
			return -1;
		}
		
		transLog = getTransactionsOf(accnt.accNumber);
		startDate.year -= yearLapse- 1; //decremented since the basis year must be below a year than current
		interest = getYearBalance(transLog, startDate) * interestRate;
	}
	
	return interest;
}

/**
	Obtains the year balance based on a starting date from the customer's transaction logs.
	Especially useful for calculating interest by obtaining year's balance and multiplying the interest rate accordingly.
	@param transLog the customer's transaction log whose year balance is to be calculated
	@parm startDate the basis date for computation; the balance is calculated up to a year from the starting date
	@return the year balance from the starting date up to one year
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
float getYearBalance(struct TransactionLog *transLog, Date startDate){
	float balance = 0;
	while(transLog != NULL){
		Transaction trans = transLog->transaction;
		
		if(trans.transDate.year - startDate.year == 1){
			// if one year has passed but month did not exceed a year
			if(trans.transDate.month < startDate.month){ 
				balance += calculateTransactionAmount(trans.transType, trans.amount);
			// if one year has passed but days did not exceed a year
			} else if((trans.transDate.month == startDate.month) && (trans.transDate.day < startDate.day)){
				balance += calculateTransactionAmount(trans.transType, trans.amount);
			}
		} 	
		transLog = transLog -> next;
	}
	
	return balance;
}

/**
	Calculates the amount depending on transaction type.
	Adds if case 2 (deposit in account); deduct if case 1,3,4 (withdraw, transfer, close resp.)
	Especially useful in getYearBalance to determine whether an amount is to be added or deducted in the total balance.
	@param transactionType the transaction type whether... (1) withdraw, (2) deposit, (3) close, or (4) transfer.
	@param amount the amount to be calculated whether to deduct or add
	@return the calculated amount
	
	@author Sumandang, AJ Ruth (12/28/2018)
*/
float calculateTransactionAmount(int transactionType, float amount){
	switch(transactionType){
		case 1:case 3:case 4:
			return -amount;
		case 2:
			return amount;
		default:
			return 0;
	}
}


// MARKPARTS


/*
	Transfer function accepts ACCOUNT NUMBER, DESTINATION ACCOUNT NUMBER and AMOUNT as inputs from the user.
	It checks the if the account is existing then check the balance if its sufficient.
	If sufficient, the function checks if the destination account is existing.
	Credits the amount then debits it to the destination account.
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
void transfer(){
	int accountNum;
	int isExisting=0;
	Transaction newTransaction;
	int accountExist=0;
	int toAccount;
	//loop for accepting inputs
	do{
		printf("Enter account number:\n");
		scanf("%d",&accountNum);

//		isExisting = checkAccount(accountNum);
		isExisting = searchAccount(accountNum).accNumber;
	
		if(isExisting!=0){
			float amount;
			int isSufficient = 0;
			do{
				printf("Enter amount to transfer:\n");
				scanf("%f",&amount);
				newTransaction.accNumber=accountNum;
				newTransaction.amount=amount;
				newTransaction.transId=transactionIdCounter+1;
				transactionIdCounter++;
				newTransaction.transType=3;
				newTransaction.transDate=getDate();
				do{
					printf("Transfer to:\n");
					scanf("%d",&toAccount);
					//		isExisting = checkAccount(accountNum);
						isExisting = searchAccount(accountNum).accNumber;
						if(accountExist!=0){
							newTransaction.toAccount=toAccount;
						}else 
							printf("Account does not exist. Retype account number.");	
				}while(accountExist=0);
					
				isSufficient = checkBalance(amount,accountNum);
			
				if(isSufficient==1){
					process(newTransaction);
					postTransfer(newTransaction);
				}else{
					printf("Your balance is not sufficient to complete the transaction.");
				}
			}while(isSufficient==0);
			
			printf("Successful\n");
			
		}else printf("\nAccount does not exist.\n");
	}while(isExisting==0);	
}

/*
	This is a helper function of Transfer function. It simply debits the amount to the destination account.
	
	@param Transaction newTransaction
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
void postTransfer(Transaction newTransaction){
	Transaction transfer;
	transfer.accNumber=newTransaction.toAccount;
	transfer.amount=newTransaction.amount;
	transfer.toAccount=NULL;
	transfer.transId=transactionIdCounter+1;
	transactionIdCounter++;
	transfer.transDate=newTransaction.transDate;
	transfer.transType=1;
	process(transfer);
}


/*
	Withdraw function accepts ACCOUNT NUMBER and AMOUNT as inputs from the user.
	It checks the if the account is existing then check the balance if its sufficient.
	If sufficient, the function validates the transaction by asking for the teller's PIN.
	Credits the amount.
	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
	@modify AJ Ruth Sumandang 12/30/2018
		(FROM) isExisting = checkAccount(accountNum); 
		(TO) isExisting = searchAccount(accountNum).accNumber;
		- Reason: because the previous function had a bug and using helper function is more optimal
*/
void withdraw(){
	int accountNum;
	
	int isExisting=0;
	Account account;
	Transaction newTransaction;
	
	do{
		printf("Enter account number:\n");
		scanf("%d",&accountNum);
		
		isExisting = searchAccount(accountNum).accNumber;
	
		if(isExisting!=0){
			float amount;
			int isSufficient = 0;
			do{
				printf("Enter amount to withdraw:\n");
				scanf("%f",&amount);
				newTransaction.accNumber=accountNum;
				newTransaction.amount=amount;
				newTransaction.transId=transactionIdCounter+1;
				transactionIdCounter++;
				newTransaction.transType=1;
				newTransaction.toAccount=NULL;
				newTransaction.transDate=getDate();
			
				isSufficient = checkBalance(amount,accountNum);
				
					if(isSufficient==1){
						int valid=0;
						do{
							valid=validate();
							if(valid==1)
								process(newTransaction);
							else 
								printf("Invalid PIN. Please try again.");	
						}while(valid==0);	
					}else{
						printf("Your balance is not sufficient to complete the transaction.");
					}
			}while(isSufficient==0);
			
			printf("Successful\n");
		
		}else printf("\nAccount does not exist.\n");
	}while(isExisting==0);		
}

/*
	Deposit function accepts ACCOUNT NUMBER and AMOUNT as inputs from the user.
	It checks the if the account is existing then then adds the transaction.
	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
void deposit(){
	
	int accountNum;
	
	int isExisting=0;
	
	while(isExisting==0){
		
		printf("Enter account number:\n");
		scanf("%d",&accountNum);
	
		//		isExisting = checkAccount(accountNum);
		isExisting = searchAccount(accountNum).accNumber;
	
		if(isExisting!=0){
			float amount;
			printf("Enter amount to deposit:\n");
			scanf("%f",&amount);
			depositAmountTo(amount, accountNum);
			
			printf("Successful\n");
			system("pause");
		}else 
			printf("\nAccount does not exist.\n");	
	}
}

/**
	Deposit an amount to the given account number.
	@param amount the amount to be deposited
	@param accountNum the account# where the money is to be deposited
	
	@author Torres, Mark
	@modified Sumandang, AJ (12/29/2018) (Note: I only transferred the code from deposit() function)
*/
void depositAmountTo(float amount, int accountNum){
	Transaction newTransaction;
	
	newTransaction.accNumber=accountNum;
	newTransaction.amount=amount;
	newTransaction.transId=transactionIdCounter+1;
	transactionIdCounter++;
	newTransaction.transType=2;
	newTransaction.toAccount=NULL;
	newTransaction.transDate=getDate();
			
	process(newTransaction);
}


/*
	checkBalance function checks if the account has sufficient balance for withdrawal.
	
	@param1 float amount
	@param2 int accountNum
	
	@return int sufficient	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
int checkBalance(float amount,int accountNum){
	Account acc;
    char str[100];
    int counter=1;
    
	acc = searchAccount(accountNum);
	if(amount < acc.balance){
		return 1;
	} else if(amount == acc.balance){
		return 1;
	} else{
		return 0;
	}
}                         


//AHJ: flaw in the algorithm
/*
	checkAccount function checks if the account exists in the file.
	
	@param int accountNum
	
	@return int isExisting	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
//int checkAccount(int accountNum){
//	FILE *fp;
//    char str[100];
//    char filename[100] = "account.in";
// 
//    fp = fopen(filename, "r");
//    if (fp == NULL){
//        printf("Could not open file %s",filename);
//        return 0;
//    }else{
//    	while (fgets(str, 1000, fp) != NULL){
//  
//    	int acc = atoi(str);
//    
//    		if(acc==accountNum){
//    			fclose(fp);	
//    			return 1;
//			}
//		}
//		return 0;
//	}      
//}

/*
	getDate function gets the system date and place it in a Dates struct.
	
	@return Dates date	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
Date getDate(){
	time_t now = time(NULL);
   	struct tm *t = localtime(&now);
   	Date date;
   	date.day=t->tm_mday;
   	date.month=t->tm_mon+1;
   	date.year=t->tm_year+1900;
   	return date;
}

/*
	Process function writes the transactions to the file.
	
	@param Transaction newTransaction
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
void process(Transaction newTransaction){
	FILE *fp;
    char str[100];
    char accNumber[100];
    itoa(newTransaction.accNumber, accNumber, 10);
    char filename[100];
	strcpy(filename, accNumber);
    strcat(filename,".tr");
 
    fp = fopen(filename, "a");
    if (fp == NULL){
        printf("Could not open file %s",filename);
    } else{
    	fprintf(fp,"%d\n",newTransaction.transId);
    	fprintf(fp,"%d\n",newTransaction.transType);
    	fprintf(fp,"%d\n",newTransaction.accNumber);
    	fprintf(fp,"%d/%d/%d\n",newTransaction.transDate.month,newTransaction.transDate.day,newTransaction.transDate.year);
    	fprintf(fp,"%f\n",newTransaction.amount);
    	fprintf(fp,"%d\n",newTransaction.toAccount);
    	fprintf(fp,"\n");
	} 	
    fclose(fp);	
    postTransaction(newTransaction.accNumber);
    updateTransId(newTransaction.transId);
}

/*
	validate function accepts the  PIN and check if it matches with the teller's PIN.
	
	@return int valid	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
int validate(){
	int pin;
	printf("VALIDATION\nEnter pin:\n");
	scanf("%d",&pin);
	if(pin==tellerPin) {
	return 1;	
	}
	else{
	return 0;
	} 
}

/*
	getLatestTransactionID function fetches the latest transaction number from a file.
	
	@return int transactionID	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
int getLatestTransactionId(){
	FILE *fp;
    char str[100];
    char filename[100] = "Metadata.mt";
    int counter=1;
 
    fp = fopen(filename, "r+");
    if (fp == NULL){
        printf("Could not open file %s",filename);
    } else{
    	while (fgets(str, 1000, fp) != NULL){
  		
  			switch(counter){
  				case 1: {
  					int latestId=atoi(str);
  					fclose(fp);	
					return latestId;
				  }
		 	}
  			counter++;
		}
		fclose(fp);	
	}
	return 0;  
   
}

/*
	updateTransId function writes the latest transactionId to a file.
	
	@param int latestTransactionId	
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
void updateTransId(int latestTransId){
	printf("udating transId....\n");
	FILE *fp;
    char filename[100] = "Metadata.mt";
 
    fp = fopen(filename, "w+");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return;
    } else{
    	fprintf(fp,"%d\n",latestTransId);
    	fclose(fp);	
	}  
}

/*
	postTransaction function calculates the new balance based on the total deposits, withdrawals and transfers.
	
	@param int accNumber
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
void postTransaction(int accNumber){
	Account accUpdated=getAccount(accNumber);
	printf("==================================================\n");
	printf("Account Number: %d\n",accUpdated.accNumber);
	printf("Name: %s,%s %s\n",accUpdated.lastName,accUpdated.firstName,accUpdated.middleName);
	printf("==================================================\n");
	printf("			Summary						\n");
	printf("==================================================\n");
	float totalDeposits=getTotal(2,accNumber);
		printf("D: %f\n",totalDeposits);
	float totalWithdrawals=getTotal(1,accNumber);
		printf("W: %f\n",totalWithdrawals);
	float totalTransfers=getTotal(3,accNumber);
		printf("T: %f\n",totalTransfers);
	
	
	float newBalance=totalDeposits-(totalWithdrawals+totalTransfers);
	
	printf("==================================================\n");	
	accUpdated.balance=newBalance;
	printf("New Balance: %f\n",accUpdated.balance);
	postAccountUpdate(accUpdated);
}

/*
	getTotal function gets the total deposits or withdrawals or transfers.
	
	@param1 int traansType
	@param2 int acctNumber
	
	@return float sum
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/

float getTotal(int transType,int acctNumber){
	FILE *fp;
    char str[100];
    char accNumber[100];
    char filename[100] ;
    int counter=1;
    Transaction transaction;
    float sum=0;
    
    itoa(acctNumber, accNumber, 10);
    strcpy(filename,accNumber);
    strcat(filename,".tr");
    
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 0;
    } else{
    	while (fgets(str, 1000, fp) != NULL){
    		switch(counter){
    			case 1:{
    				transaction.transId=atoi(str);
    				counter++;
					break;
				}
				case 2:{
					transaction.transType=atoi(str);
					counter++;
					break;
				}
				case 3:{
					transaction.accNumber=atoi(str);
					counter++;
					break;
				}
				case 4:{
					//DUMMY DATE (must call date transform function)
					transaction.transDate=getDate();
					counter++;
					break;
				}
				case 5:{
					transaction.amount=atof(str);
					if(transaction.transType==transType){
					sum=sum+atof(str);
					counter++;
					break;
					}
				}
				case 6:{
					transaction.toAccount=atoi(str);
					counter=0;
					break;
				}
				case 0:{
					counter=1;		
					break;
				}
			}	
		}
		fclose(fp);
		return sum;
	} 
}
/*
	getaccount function retrieves the Account specified by the account number.
	
	@param int accountNum
	
	@return Account account
	
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
Account getAccount(int accountNum){
	Account acc;
	
	FILE *fp;
    char str[100];
    char filename[100] = "account.in";
    int counter=1;
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
    } else{
    	acc = searchAccount(accountNum);
		fclose(fp);
	} 
	return acc; 
}                
