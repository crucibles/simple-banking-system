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
#define SAVINGS_DEFAULT 200
#define CURRENT_DEFAULT 1000

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

//GUI functions
void gotoxy(int x, int y);
void hline(int,int,int);
void rect();
void rect(int,int,int,int);
void start();
void menu();
void vline(int,int,int);
void loadingScreen();
void deleteInput(int num);

//General helper functions
void convertDateStringToDate(char *dateString, int *month, int *day, int *year);
int getLatestIdFromStorage(int type);
int getUserInput();
float getUserFloatInput();
int *getUserDateInput();
char* fgetsW(char* buffer, int bufferLength, FILE *fp);
Date getSystemDate();
int confirm();
int count();

//Account functions
void createAccount();
void closeAccount();
void displayAcc(int accNumber);
Account getAccount();
Account searchAccount(int accNumber);
void updateAccount();
//Account helper functions
int getLatestAccountId();
char *newPin();
void postAccount(Account);
void postAccountUpdate(struct Account currAccount);
void updateMenu(Account currAccount);
void updateProcess(int choice, struct Account currAccount);

//Transaction Functions
struct TransactionLog *getTransactionsOf(int accNumber);
void deposit();
void depositAmountTo(float amount, int accNumber);
void transactions();
void transfer();
void displayTransactionLogs();
void withdraw();

//Transaction helper functions
void askAccNum();
float calculateTransactionAmount(int transType, float amount);
int checkBalance(float amount,int accountNum);
Account getAccount(int accountNum);
float getInterest(Account currAccount);
int getLatestTransactionId();
struct TransactionLog *getTransactions();
float getTotal(int transType,int accNumber);
float getYearBalance(struct TransactionLog *transLog, Date startDate);
void postTransaction(int accNumber);
void postTransfer(Transaction newTransaction);
void process(Transaction newTransaction);
void updateIds(int latestTransId, int latestAccId);
int validate();
int withdrawAmountFrom(float, int);

//Security functions
bool dateChecker(int,int,int);
bool intChecker();


int transactionIdCounter=getLatestTransactionId();
int tellerPin=1234;

main(){
	loadingScreen();
	start();
	
	return 0;
}


void loadingScreen(){
	int x = 1;
	
	gotoxy(15,15);
	system("color 2");
	printf("Loading: ");
	
	for(int i = 23;i <= 56; i++){
		gotoxy(i,15);
		Sleep(30);
        printf("\xB2");
        gotoxy(57,15);
        printf(" [%d %]", x);
        x+=3;
    }
    printf("\n");
    gotoxy(25,19);
    system("pause");
    system("cls");
}


/*
	Checks date if it is valid.
	The date is valid if...
	- Years should be greater than 1900 OR less than 9999
	- Months should be 1 <= m <= 12 where m = months
	- Months of 30 should have 1 <= d <= 30 where d = days
	- Months of 31 should have 1 <= d <= 31 where d = days
	- February should be 1 <= d <= 28 OR 1 <= d <= 29, if normal or leap year respectively.
	Otherwise, it will prompt user which part of the date is invalid
	@params 
		month - month of the date to be checked 
		day - day of the date to be checked 
		year - year of the date to be checked 
	@return 
		true if valid date; false if not
	
	@author Cedric Alvaro (12/28/2018)
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
				return false;	
			}
                
        } else{
            return false;
        }
        
    } else{
        return false;
    }

}

/*
	Prints the transaction menu, and a list of choices. It will call those functions then.
	
	@author Cedric Alvaro (12/28/2018)
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
	"0.Go back to Main Menu\n\n\n\n\n\n\n\n\t"
	"[Note: -1 Main Menu] ==Enter your choice==");
	
	rect(4, 4, 77, 15);
	gotoxy(35,18);
	choice = getUserInput();
    system("cls");

    switch(choice){
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
        default:
        	transactions();
        	break;
    }
}


/*
	Draws a rectangle.
	@params
		x1 - the starting of the horizontal lines
		x2 - the end of the horizontal lines
		y1 - the starting of vertical lines
		y2 - the ending of vertical lines
		
	@author Cedric Alvaro
*/
void rect(int x1,int y1,int x2, int y2){
  /* printing corners */
  gotoxy(x2, y1);
  printf("%c", 191);
  gotoxy(x1, y2);
  printf("%c", 192);
  gotoxy(x2, y2);
  printf("%c", 217);
  gotoxy(x1, y1);
  printf("%c", 218);
  /* printing lines */
  hline(x1 + 1, x2 - 1, y1);
  hline(x1 + 1, x2 - 1, y2);
  vline(y1 + 1, y2 - 1, x1);
  vline(y1 + 1, y2 - 1, x2);
}


/*
	Draws a horizontal line. 
	@params
		x1 - the starting of the horizontal line
		x2 - the end of the horizontal line
		y - the position base on y-axis where to put that horizontal line
		
	@author Cedric Alvaro (12/28/2018)
*/
void hline(int x1, int x2, int y){
	for(int i = x1; i <= x2; i++){
		gotoxy(i, y);
		printf("%c", 196);
	}
}

/*
	Draws a vertical line.
	@params
		y1 - the starting of the vertical line
		y2 - the end of the vertical line
		x - the position base on x-axis where to put that vertical line
		
	@author Cedric Alvaro
*/
void vline(int y1, int y2, int x){
	for(int i = y1; i <= y2; i++){
    	gotoxy(x, i);
		printf("%c", 179);
	}
}

/*
	Prints a series of actions for the teller/user to choose from.
	The function will then called the functions necessary for the user's choice.
	
	@author Cedric Alvaro
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
	"7.Logout\n\t\t"
	"0.Exit Program\n\n\n\n\t"
	"[Note: -1 Main Menu]  ==Enter your choice==");
	rect(4, 4, 77, 15);
	
	gotoxy(35,18);
	choice = getUserInput();
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
	    	displayTransactionLogs();
	       	break;
	    case 7:
	       	start();
        	break;
		case 0:
			exit(1);
			break;     
    }
    menu();	
}


/*
	Asks the teller to input the manager password for role-locked actions such as delete, etc.
	@returns 
		An integer that will determine if the inputted password is correct.
		1 : true
		0: false
	
	@author Cedric Alvaro (12/28/2018)
	@modified Sumandang, AJ Ruth (12/30/2018)
		- (FROM) while(input != '\r'){...}
		- (TO) do{...}while(input != '\r')
		- Reason(s): To remove the double input from user.
*/
int confirm(){
	char password[] = "manager";
	char *pass = new char[1024];
    int i = 0, tries = 3;

    while(tries > 0){
	    system("cls");
		printf("\n\n\t\t\tSIMPLE BANKING MANAGEMENT SYSTEM");
		system("color 6");
		rect(5, 7, 75, 15);
		gotoxy(20, 10);
		printf("\tPassword to confirm this action: ");
		char input = getch();
		while (input != '\r' && i <= 10)
		{
			if (!(input == 127 || input == 8)) {
				printf("*");
				pass[i] = input;
				i++;
			} else if (i != 0){
				printf("\b \b");
				i--;
			}
			input = getch();
		}
		pass[i] = '\0';

	
		if (strcmp(pass, password) == 0){
    	gotoxy(24, 18);
    	
		printf("Processing ");
		for(i = 0; i <= 6; i++){
			Sleep(200);
            printf(".");
        }
        
        return 1;
	    } else{
	    	tries--;
			gotoxy(25, 19);
			printf("Password is wrong! Remaining tries: %d", tries);
		
			for(i = 0; i <= 6; i++){
				Sleep(200);
	            printf(".");
	        }
		}
	}
	
	return 0;
}


/*
	Asks for the account number of the user to be deleted. 
	It deletes the info from the accounts.in.
	It will also auto withdraw all his/her balance before closing the account.
	
	@author Cedric Alvaro (12/28/2018)
*/
void closeAccount(){
	int remAcc;
	int counter = count();
	Account searchedAcc, user;
	char choice;
	char blank[100];
    
    printf("\n\n\n\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 CLOSING AN ACCOUNT \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n");
    printf(" \tEnter the account no. of the user you want to delete: ");
    remAcc = getUserInput();
    user = getAccount(remAcc);
    system("cls");
	displayAcc(remAcc);
	
	if(user.accNumber > 0){
		
	    fflush(stdin); 
	    printf("\n\tAre you sure you want to delete this account?\n"
	    "\tYou will automatically withdraw all your remaining balance.\n"
		"\tYou cannot undo this action (Y/N): ");
	    scanf("%c", &choice);
	    
	    if(choice == 'y' || choice == 'Y' ){
	    	
	    	if (confirm() && withdrawAmountFrom(user.balance, user.accNumber)){
	    		
				FILE *fr, *newrec;
	    		fr = fopen("account.in", "r");
	    		newrec = fopen("new_account.in", "w");
	    
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
			system("cls");
			printf("\n\tYour choice resulted in\n\tgoing back to the main menu!");
			system("pause");
			return;
		}
	
	}
    
}


/*
	Tells the teller to input his/her password first before he/she can use the system.
	
	@author Cedric Alvaro (12/28/2018)
*/
void start(){
	char pass[1024];
	char password[] = "teller";
    int i = 0, x = 0, key;
    
	printf("\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 SIMPLE BANKING MANAGEMENT SYSTEM \xB2\xB2\xB2\xB2\xB2\xB2\xB2");
	system("color 6");
	
	rect(5, 7, 75, 15);
	gotoxy(20, 10);
	printf("\tEnter the password to login: ");
	
	char input = getch();
	while (input != '\r' && i <= 10)
	{
		if (!(input == 127 || input == 8)) {
			printf("*");
			pass[i] = input;
			i++;
		} else if(i != 0){
			printf("\b \b");
			i--;
		}
		input = getch();
	}
	pass[i] = '\0';
	
    if(strcmp(pass, password) == 0){
    	gotoxy(24, 18);
    	
		printf("Preparing the system for you ");
		for(i = 0; i <= 6; i++){
			Sleep(50);
            printf(".");
        }
        
		system("cls");
		menu();
    } else {
    	gotoxy(25, 15);
		printf("Password is wrong! Input again.\n\n");
		gotoxy(25, 18);
		system("pause");
		system("cls");
		start();
	}
    
}


/*
	Moves the cursor to the given coordinates.
	@params
		x - the x-axis (horizontal)
		y - the y-axis (vertical)
		
	@author Cedric Alvaro (12/28/2018)
*/
void gotoxy(int x, int y){

	static HANDLE h = NULL;
	if(!h){
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	COORD c = {x, y};
	SetConsoleCursorPosition(h, c);
}

/*
	Finds the line number of accNumber to be searched in the account file, 
	since accNumber is the unique and unchangeable attribute in each bank account.
	@params 
		accNumber - used and searched in the account.in file
	@returns 
		the line number of that accNumber in accounts.in file
	
	@author Cedric Alvaro (12/28/2018)
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
	An fgets wrapper function.
	It modifies the fgets function by removing the /n at the end (if it exists).
	Takes the same parameters as fgets.
	@param 
		buffer - the string to put edits
		bufferLength - the length of buffer
		file - file pointer
	@return 
		the resulting string if successful; return null pointer constant, 0, if unsuccessful.
	
	@author Sumandang, AJ Ruth H. (12/28/2018)
*/
char *fgetsW(char *buffer, int bufferLength, FILE *fp){
	if (fgets(buffer, bufferLength, fp) != 0){
        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n'){
        	buffer[len - 1] = '\0';
		}
        return buffer;
    }
    return 0;
}


/*
	Searches an account element from the list of accounts in accounts.in.
	@params 
		accNumber - the account to be searched, since it is the unique attribute
	@return 
		A struct Account instance of the searched account; 
		If not found, returns Account instance with accNumber = 0 (means not a valid account)
		
	@author Cedric Alvaro (12/28/2018)
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
	Asks input accNumber to be shown as account.
	
	@author Cedric Alvaro (12/28/2018)
*/
void askAccNum(){
	int accNumber;
	
	printf("\n\n\n\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 SEARCH AN ACCOUNT \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n");
	printf(" \tEnter the account number: ");
	scanf("%d", &accNumber);
	system("cls");
	displayAcc(accNumber);
}

/*
	Asks the teller to input the account number of the user, then searches it from the accounts.in
	It will then print the details of that account.

	@author Cedric Alvaro (12/28/2018)
*/
void displayAcc(int accNumber){
	Account user;
	user = searchAccount(accNumber);
	
	printf("\n\n\n\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 ACCOUNT DETAILS \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n");
	
	if(user.accNumber != 0){
		printf(
			" \tHere are the account details:\n\n"
			
			" \tFirst name:		%s"
			" \tMiddle name:		%s"
			" \tLast name:		%s"
			" \tBirthDate (MM/DD/YY):	%d/%d/%d\n"
			" \tBalance:		%.2f\n"
			" \tAccount Type:		%s\n"
			" \tAccount Created (MM/DD/YY): %d/%d/%d\n"
			" \tAccount Number:		%d\n"
			" \tAccount Pin:		%s\n\n",
			
			user.firstName, 
			user.middleName, 
			user.lastName, 
			user.birthDate.month, user.birthDate.day, user.birthDate.year, 
			user.balance + getInterest(user), 
			user.accType==0? "savings": "current", 
			user.startDate.month, user.startDate.day, user.startDate.year, 
			user.accNumber, 
			user.pin
		);
	} else {
		
		printf(" \t The user does not exist.\n");
	}
	printf("\t");
	system("pause");
}


/*
	Creates an account for a user then writes its information to a file named "account.in".
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
	@params
		add - the account to be added into the file
	
	@author Cedric Alvaro (12/28/2018)
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
	printf("\n\t\t\t\xB2 This is the newly created account \xB2");
	displayAcc(add.accNumber);
}

/**
	Gets input date from user.
	Includes error checking.
	@returns 
		array of date where...
			date[0] -> month
			date[1] -> day
			date[2] -> year
			
	@author Cedric Alvaro (12/30/2018)
	@modified AJ Ruth Sumandang (12/31/2018)
		- Moved some of the code to convertDateStringToDate() for modularity
*/
int *getUserDateInput(){

	int *date = new int[1024];
    char *input = new char[1024];

	do{
		date[0] = 0;
		date[1] = 0;
		date[2] = 0;
		
		printf("\33[2K\r");
		printf("\tPlease input a valid Date: ");
		fgetsW(input, 100, stdin);
		deleteInput(1);
				
		convertDateStringToDate(input, &date[0], &date[1], &date[2]);			    
	}while(!dateChecker(date[0], date[1], date[2]));
	
	return date;
}

/**
	Converts the received datestring to date & puts the result in month, day, year variable.
	@params
		dateString - the string to be converted
		month - where the month result of the datestring is to be placed
		day - where the day result of the datestring is to be placed
		year - where the year result of the datestring is to be placed
		
	@author Cedric Alvaro (12/30/2018)
	@modified AJ Ruth Sumandang (12/31/2018)
		- Moved this code from getUserDateInput for modularity
*/
void convertDateStringToDate(char *dateString, int *month, int *day, int *year){
	int i = 0;
	char s[2] = "/";
    char *token;
    int date[3];
    
	/* get the first token */
	token = strtok(dateString, s);
    /* walk through other tokens */
    while( token != NULL ) {
		date[i] = atoi(token);
	   	token = strtok(NULL, s);
	   	i++;
	};
	*month = date[0];
	*day = date[1];
	*year = date[2];
}

/*
	Asks for input to a new account.
		
	@author Cedric Alvaro
*/
void createAccount(){
	Account add;


	int counter = 0;
	int *date = new int[3];
	char localDate[9];

	
	printf(
	"\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2"
	" CREATION OF ACCOUNT "
	"\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n\n"
	);
	
	char *interact[6] = {
	"	Enter first name: ", 
	"	Enter middle name: ", 
	"	Enter last name: ", 
	"	Enter birthdate in MM/DD/YYYY format: \n", 
	"	\tChoose what account type:\n\t[0 - savings] 200 initial required \n\t[1 - current] 1000 initial required\t",
	"	Enter starting amount: ",
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
				date = getUserDateInput();   
				add.birthDate.month = date[0];
				add.birthDate.day = date[1];
				add.birthDate.year = date[2];
			
				break;
			case 4:
				do {
					add.accType = getUserInput();
					printf("\033[A");
				}while(add.accType != 0 && add.accType != 1);
				printf("\n\n");
				break;
			case 5:
				float def = add.accType==1? CURRENT_DEFAULT: SAVINGS_DEFAULT;

				do{
					add.balance = getUserFloatInput();
				} while(add.balance < def);
					
				break;
			
		}
			
    	counter++;
	}
	

	_strdate(localDate);
	add.startDate.month = atoi(localDate);
	add.startDate.day = atoi(&localDate[3]);
	add.startDate.year = atoi(&localDate[6]);
	
	add.accNumber = getLatestAccountId();
	strcpy(add.pin, newPin());
	printf("\n\t");
	
    postAccount(add);
    depositAmountTo(add.balance, add.accNumber);
    system("cls");
    
}


/*
	Finds the next account number based on the last used account number then increments it by one.
	@returns 
		An integer that is the account number of a newly created account.
		
	@author Cedric Alvaro (12/28/2018)
	@modified AJ Ruth Sumandang (12/28/2018)
		- (FROM) obtaining the latest id by finding the latest account id in account.in
		- (TO) storing the latest id in the Metadata.mt instead
		- Reason: because if the system deletes the latest account, there would be a duplicate account #
*/
int getLatestAccountId(){
	return getLatestIdFromStorage(2) + 1;
}

/*
	Generates a new pin based on the last pin used just incrementing it by one.
	@returns 
		An integer that will be the pin of a newly created account.
	
	@author Cedric Alvaro (12/28/2018)
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
	} while(line != NULL);
	
	counter /= 10;
	counter++;
	
	fclose(fr);
	itoa(counter, pin, 10);
	system("pause");
	return pin;
}

/*
	Counts how many users are there in the banking system
	@returns 
		An integer that is the account number of a new created account.
		
	@author Cedric Alvaro (12/28/2018)
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
	
	@author AJ Ruth Sumandang (12/28/2018)
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
	Displays the customer's current information that can be modified.
	@params
		currAccount - the account whose data is to be displayed.
	
	@author AJ Ruth Sumandang (12/28/2018)	
*/
void updateMenu(struct Account currAccount){

    printf("\n\n\n\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 UPDATING OF ACCOUNT \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n");
	printf(" \tInput number you want to update...\n");
	printf("\t[1] First name: 	%s", currAccount.firstName);
	printf("\t[2] Middle name: 	%s", currAccount.middleName);
	printf("\t[3] Last name: 		%s", currAccount.lastName);
	printf("\t[4] Birthdate(M/D/Y): 	%d/%d/%d\n", currAccount.birthDate.month, currAccount.birthDate.day, currAccount.birthDate.year);
	printf("\t[5] PIN: 		%s\n", currAccount.pin); 
	printf("\t[0] No changes. Exit to main menu.\n\n", currAccount.pin);
	printf(" \tInput number: ");
}

/**
	Contains the processes necessary for updating current account.
	@params 
		choice - user choice; (1) first name, (2) middle name, (3) last name, (4) birthdate, (5) PIN
		currAcount - variable containing the current account to be updated
	
	@author AJ Ruth Sumandang (12/28/2018)
*/
void updateProcess(int choice, struct Account currAccount){
	char name[100];
	char pin[6];
	struct Date birthdate;
	int *date;
	
	switch(choice){
		date[0] = 0;
		date[1] = 0;
		date[2] = 0;
		case 1:
			printf(" \t\tEnter First Name (change): ");
			fgetsW(name, BUFFER_SIZE, stdin);
			strcpy(currAccount.firstName, name);
			strcat(currAccount.firstName, "\n");
			postAccountUpdate(currAccount);
			break;
		case 2:
			printf(" \t\tEnter Middle Name (change): ");
			fgetsW(name, BUFFER_SIZE, stdin);
			strcpy(currAccount.middleName, name);
			strcat(currAccount.middleName, "\n");
			postAccountUpdate(currAccount);
			break;
		case 3:
			printf(" \t\tEnter Last Name (change): ");
			fgetsW(name, BUFFER_SIZE, stdin);
			strcpy(currAccount.lastName, name);
			strcat(currAccount.lastName, "\n");
			postAccountUpdate(currAccount);
			break;
		case 4:
			printf(" \t\tEnter Birthdate(change)(MM/DD/YYYY format): ");
			date = getUserDateInput();
			birthdate.month = date[0];
			birthdate.day = date[1];
			birthdate.year = date[2];
			currAccount.birthDate.month = birthdate.month;
			currAccount.birthDate.day = birthdate.day;
			currAccount.birthDate.year = birthdate.year;
			postAccountUpdate(currAccount);
			break;
		case 5:
			printf(" \t\tEnter PIN (change): ");
			fgetsW(pin, 6, stdin);
			pin[6] = '\0';
			strcpy(currAccount.pin, pin);
			postAccountUpdate(currAccount);
			break;
		case 0:
			break;
		default:
			printf(" \t\tInvalid input! Please input again...\n");
			system("pause");
			break;
	}
}

/**
	Updates the account file (account.in).
	@param 
		currAccount - the account to be updated.
	
	@author AJ Ruth Sumandang (12/28/2018)
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
        			sprintf(temp, "%d", currAccount.birthDate.day);
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
        			sprintf(lineString, "%.2f", currAccount.balance);
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
    	printf("(Err1) Error in editing the file. Please contact developer for help.\n");
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
	@returns
		Account instance; the searched account if found, but account of accNumber = 0 if input acc# is not found and user has given up.
		
	@author AJ Ruth Sumandang (12/28/2018)
*/
Account getAccount(){
	int choice;
	int accNumber;
	Account currAccount;
    printf("\n\n\n\t\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 UPDATING OF ACCOUNT \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n");
	
	do{
		choice  = 0;
		printf(" \tPlease input account number to update: ");
		accNumber = getUserInput();
		currAccount = searchAccount(accNumber);
	
		if(currAccount.accNumber == 0){
			printf(" \tInvalid input or the account does not exist. Do you want to input again or exit to main menu?\n");
			printf(" \t[1] Input Again");
			printf(" \t[0] Exit to Main Menu");
		}
	} while(choice != 0 && choice != 1);
	
	system("cls");
	return currAccount;
}

/**
	Retrieved user's input.
	@returns
		the user choice input
	
	@author AJ Ruth Sumandang (12/28/2018)
*/
int getUserInput(){
	int choice = 0;
	int retval, count = 0;
	
	printf("\n");
	while(choice == 0){
		
		printf("\33[2K\r");
		printf("\tPlease enter a valid number: ");
		choice = scanf("%d", &retval);
		deleteInput(1);
		
    	while(getchar() != '\n');
	}
	
	if(retval == -1){
		menu();
	}
	
	return retval;
}

void deleteInput(int num){

	for( int i=0; i<num; i++ ){
		printf("\b");
	}

}

float getUserFloatInput(){
	int choice = 0;
	float retval;
	
	printf("\n");
	while(choice == 0){
		printf("\33[2K\r");
		printf("\tPlease enter a valid amount: ");
    	choice = scanf(" %f", &retval);
    	deleteInput(1);
    	
    	while(getchar() != '\n');
	}
	
	return retval;
}

/**
	Allows the user to view an existing customer's transaction.
	Contains the function necessary for viewing transaction logs such as getting transaction(in struct form).
	
	@author AJ Ruth Sumandang (12/28/2018)
*/
void displayTransactionLogs(){
	TransactionLog *transactionLog;
	Transaction transaction;
	transactionLog = getTransactions();	
	
	while(transactionLog != NULL){
		transaction = transactionLog -> transaction;
		
		char transType[][20] = {"Withdrawal", "Deposit", "Closing Account"};
		if(transaction.accNumber != 0){
			printf(" Transaction ID: 	%d\n", transaction.transId);
			printf(" Account Number: 	%d\n", transaction.accNumber);
			printf(" Transaction Date: 	%d/%d/%d\n", transaction.transDate.month, transaction.transDate.day, transaction.transDate.year);
			printf(" Transaction Type: 	%s\n", transType[transaction.transType - 1]);
			printf(" Transaction Amount:	%.2f\n", transaction.amount);
			(transaction.toAccount != 0)? printf("Transferred to Accnt#:	%d\n\n", transaction.accNumber): printf("\n\n");
		}
		
		transactionLog = transactionLog -> next;
	}
	system("pause");
}

/**
	Retrieves a customer's transaction from the files.
	@returns 
		customer transaction logs (in struct Transaction form)
	
	@author Sumandang, AJ Ruth (12/28/2018)
	@modify Cedric Alvaro (12/30/2018) a
		- added condition in while loop
*/
struct TransactionLog *getTransactions(){
	int choice = 0, accNumber;
	struct TransactionLog *transLog = new TransactionLog;
	transLog = NULL;
	
	printf(
	"\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2"
	" TRANSACTION LOGS MANAGEMENT "
	"\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n\n"
	);
	
	do{
		choice = 0;
		printf(" \tInput account number to view: ");
		accNumber = getUserInput();
		transLog = getTransactionsOf(accNumber);
		
		if(transLog == NULL){
			printf("\n\n \tInvalid input or non-existent account. Do you want to input again?\n");
			printf(" \t[1] Input Again\n");
			printf(" \t[0] Exit to Main Menu\n");
			printf(" \tInput choice: ");
			choice = getUserInput();
		} 
	} while(choice != 0 && choice!= 1);
	
	return transLog;
}

/**
	Gets the transaction logs of customer given an account #.
	@params 
		accNumber - the account # of the customer whose transaction logs are to be retrieved
	@returns
		An array of customer's transaction logs
	
	@author AJ Ruth Sumandang (12/28/2018)
*/
struct TransactionLog *getTransactionsOf(int accNumber){
	FILE *fr;
	char accountFileName[100];
	
	struct TransactionLog *transLog = new TransactionLog;
	transLog = NULL;
	sprintf(accountFileName, "%d", accNumber);
	strcat(accountFileName, ".tr");
	
	fr = fopen(accountFileName, "r");
	
	if(fr == NULL){
//		printf("(err3) File account %s not found.\n", accountFileName);
		return NULL;
	}
	
	char *line = new char[1024];
	int i = 1;
	struct Transaction transaction;
	
	while(fgetsW(line,BUFFER_SIZE, fr) != NULL){
		switch(i){
			case 1:
    			transaction.transId = atoi(line);
				break;
			case 2:
				transaction.transType = atoi(line);
				break;
			case 3:
				transaction.accNumber = atoi(line);
				break;
			case 4:
				convertDateStringToDate(line, &transaction.transDate.month, &transaction.transDate.day, &transaction.transDate.year);
				break;
			case 5:
				transaction.amount = atof(line);
				break;
			case 6:
				transaction.toAccount = atoi(line);
				break;
			case 7:
				struct TransactionLog *newLog = new TransactionLog;
				newLog->transaction = transaction;
				newLog->next = transLog;
				transLog = newLog;
				i = 0;
				break;
		}
		i++;
	}
	
//	do{
//		fscanf(fr, "%d", &transaction.transId);
//		fscanf(fr, "%d", &transaction.transType);
//		fscanf(fr, "%d", &transaction.accNumber);
//		fscanf(fr, "%d/%d/%d", &transaction.transDate.month, &transaction.transDate.day, &transaction.transDate.year);
//		fscanf(fr, "%f", &transaction.amount);
//		fscanf(fr, "%d", &transaction.toAccount);
//		fgets(line, 100, fr);
//				
//		struct TransactionLog *newLog = new TransactionLog;
//		newLog->transaction = transaction;
//		newLog->next = transLog;
//		transLog = newLog;
//	} while(!feof(fr) && line != NULL);
	
	fclose(fr);
	return transLog;
}



/**
	Gets the interest of a given customer account.
	@params
		accnt - the customer account whose interest is to be calculated
	@return 
		The total interest of customer from account-creation date to present
	
	@author AJ Ruth Sumandang (12/28/2018)
*/
float getInterest(Account accnt){
	Date startDate = accnt.startDate;
	Date currDate = getSystemDate();
	
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
		float yearBalance = 0;
		for(int i = 1; i <= yearLapse; i++){
			transLog = getTransactionsOf(accnt.accNumber);
			startDate.year -= i - 1; //decremented since the basis year must be below a year than current
			yearBalance = getYearBalance(transLog, startDate);
			interest +=  yearBalance * interestRate;
			startDate.year++;
		}
	}
	
	return interest;
}

/**
	Obtains the year balance based on a starting date from the customer's transaction logs.
	Especially useful for calculating interest by obtaining year's balance and multiplying the interest rate accordingly.
	@params
		transLog - the customer's transaction log whose year balance is to be calculated
		startDate - the basis date for computation; the balance is calculated up to a year from the starting date
	@return 
		The year balance from the starting date up to one year
	
	@author AJ Ruth Sumandang (12/28/2018)
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
		} else if(trans.transDate.year == startDate.year){
			// if same year but more than the starting month
			if(trans.transDate.month > startDate.month){ 
				balance += calculateTransactionAmount(trans.transType, trans.amount);
			// if same year, month, and day on or after the starting day
			} else if((trans.transDate.month == startDate.month) && (trans.transDate.day >= startDate.day)){
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
	@param 
		transactionType - the transaction type whether... (1) withdraw, (2) deposit, (3) close, or (4) transfer.
		amount - the amount to be calculated whether to deduct or add
	@return 
		The calculated amount; positive if deopsit & negative if withdraw, transfer or close
	
	@author AJ Ruth Sumandang (12/28/2018)
*/
float calculateTransactionAmount(int transactionType, float amount){
	switch(transactionType){
		case 1: case 3: case 4:
			return -amount;
		case 2:
			return amount;
		default:
			return 0;
	}
}

/*
	Transfer function accepts ACCOUNT NUMBER, DESTINATION ACCOUNT NUMBER and AMOUNT as inputs from the user.
	It checks the if the account is existing then check the balance if its sufficient.
	If sufficient, the function checks if the destination account is existing.
	Credits the amount then debits it to the destination account.
	
	@author Mark Torres 12/28/2018
	@modify Cedric Alvaro 12/30/2018 added input security
*/
void transfer(){
	int accountNum;
	int isExisting = 0;
	int accountExist = 0;
	int toAccount;
	
	printf("\n\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 TRANSACTIONS: TRANSFER MONEY \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n");
	
	//loop for accepting inputs
	do{
		Transaction newTransaction;
		printf("\t==Enter account number==");
		accountNum = getUserInput();

//		isExisting = checkAccount(accountNum);
		isExisting = searchAccount(accountNum).accNumber;
	
		if(isExisting != 0){
			float amount;
			int isSufficient = 0;
			do{
				printf("\t\t==Enter amount to transfer==");
				amount = getUserFloatInput();
				newTransaction.accNumber = accountNum;
				newTransaction.amount = amount;
				newTransaction.transId = transactionIdCounter + 1;
				transactionIdCounter++;
				newTransaction.transType = 3;
				newTransaction.transDate = getSystemDate();
				do{
					printf("\t==Transfer to==");
					toAccount = getUserInput();
					//		isExisting = checkAccount(accountNum);
						isExisting = searchAccount(accountNum).accNumber;
						if(accountExist != 0){
							newTransaction.toAccount = toAccount;
						} else{
							printf("\tAccount does not exist. Retype account number.");
						}
					
					printf("33]2k\r");			
				} while(accountExist == 0);
					
				isSufficient = checkBalance(amount,accountNum);
			
				if(isSufficient == 1){
					process(newTransaction);
					postTransfer(newTransaction);
				} else{
					printf("\tYour balance is not sufficient to complete the transaction.");
				}
			} while(isSufficient == 0);
			
			printf("\tSuccessful\n");
			
		} else printf("\nAccount does not exist.\n");
	} while(isExisting == 0);	
}

/*
	Helper function of Transfer function. It simply debits the amount to the destination account.
	@param 
		newTransaction - the transaction to be added to the transaction file.
	
	@author Mark Torres (12/28/2018)	
*/
void postTransfer(Transaction newTransaction){
	Transaction transfer;
	transfer.accNumber = newTransaction.toAccount;
	transfer.amount = newTransaction.amount;
	transfer.toAccount = NULL;
	transfer.transId = transactionIdCounter+1;
	transactionIdCounter++;
	transfer.transDate = newTransaction.transDate;
	transfer.transType = 1;
	process(transfer);
}

/*
	Withdraw function accepts ACCOUNT NUMBER and AMOUNT as inputs from the user.
	It checks the if the account is existing then check the balance if its sufficient.
	If sufficient, the function validates the transaction by asking for the teller's PIN.
	Credits the amount.
	
	@author Mark Torres (12/28/2018)
	@modify Mark Torres (12/28/2018)	
	@modify AJ Ruth Sumandang (12/30/2018)
		(FROM) isExisting = checkAccount(accountNum); 
		(TO) isExisting = searchAccount(accountNum).accNumber;
		- Reason: because the previous function had bugs and using helper function is more optimal
*/
void withdraw(){
	int accountNum;
	
	int isExisting = 0;
	Account account;
	Transaction newTransaction;
	
	printf("\n\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 TRANSACTIONS: WITHDRAWAL \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
	
	do{
		printf("\n\t==Enter account number==");
		accountNum = getUserInput();
		
		isExisting = searchAccount(accountNum).accNumber;
	
		if(isExisting != 0){
			float amount;
			int isSufficient = 0;
			do{
				printf("\t\t==Enter amount to withdraw==");
				amount = getUserFloatInput();
				newTransaction.accNumber = accountNum;
				newTransaction.amount = amount;
				newTransaction.transId = transactionIdCounter+1;
				transactionIdCounter++;
				newTransaction.transType = 1;
				newTransaction.toAccount = NULL;
				newTransaction.transDate = getSystemDate();
			
				isSufficient = checkBalance(amount,accountNum);
				
					if(isSufficient == 1){
						int valid = 0;
						do{
							valid = validate();
							if(valid == 1){
								process(newTransaction);
								system("pause");
							} else{
								printf("\n\tInvalid PIN. Please try again.");
							}
									
						} while(valid == 0);	
					} else{
						printf("\tYour balance is not sufficient to complete the transaction.");
					}
			} while(isSufficient == 0);
			
			printf("\tSuccessful\n");
		
		} else printf("\t Account does not exist.");
		
		printf("\33[2K\r");
		printf("\33[2A");
	}while(isExisting == 0);		
}

/*
	Withdraw helper function, withdraws amount from an account number, and an amount
	@params
		amt amount to be withdrawn
		accNum account Number of the user where the money will be withdrawn

	@author Mark Torres 12/28/2018
	@modified Cedric Alvaro 12/30/2018
	just copied some code to this helper function
	
*/
int withdrawAmountFrom(float amount, int accountNum){
	Account account;
	Transaction newTransaction;
	int isSufficient = 0;
	
	printf("\n\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 TRANSACTIONS: WITHDRAWAL \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n");
	
	do{
	
		newTransaction.accNumber = accountNum;
		newTransaction.amount = amount;
		newTransaction.transId = transactionIdCounter+1;
		transactionIdCounter++;
		newTransaction.transType = 1;
		newTransaction.toAccount = NULL;
		newTransaction.transDate = getSystemDate();
			
		isSufficient = checkBalance(amount, accountNum);
				
		if(isSufficient == 1){
			int valid = 0;
			
			do{
				valid = validate();
				if(valid == 1){
					process(newTransaction);
					return 1;
				} else{
					printf("Invalid PIN. Please try again.");
				}
									
			}while(valid == 0);
				
		} else{
			printf("Your balance is not sufficient to complete the transaction.");
			return 0;
		}
		
	} while(isSufficient == 0);
	
}

/*
	Deposit function accepts ACCOUNT NUMBER and AMOUNT as inputs from the user.
	It checks the if the account is existing then then adds the transaction.
	
	@author Mark Torres 12/28/2018
	@modified Cedric Alvaro 12/30/2018 just format of if - else	
*/
void deposit(){
	
	int accountNum;
	int isExisting=0;
	
	printf("\n\n\n\t\t\xB2\xB2\xB2\xB2\xB2\xB2\xB2 TRANSACTIONS: DEPOSIT \xB2\xB2\xB2\xB2\xB2\xB2\xB2\n\n");
	
	while(isExisting==0){
		
		printf("\t==Enter account number==");
		accountNum = getUserInput();
	
		//		isExisting = checkAccount(accountNum);
		isExisting = searchAccount(accountNum).accNumber;
	
		if(isExisting != 0){
			float amount;
			printf("\tEnter amount to deposit: ");
			amount = getUserFloatInput();
			depositAmountTo(amount, accountNum);
			
			printf("\tSuccessful\n");
			system("pause");
		} else 
			printf("\nAccount does not exist.\n");	
	}
}

/**
	Deposit an amount to the given account number.
	@param 
		amount - the amount to be deposited
		accountNum - the account# where the money is to be deposited
	
	@author Mark Torres (12/28/2018)
	@modified AJ Ruth Sumandang (12/29/2018)
		- Only transferred the code from deposit() function
		- Reason: for modularity; so this function can be used in createAccount() 
*/
void depositAmountTo(float amount, int accountNum){
	Transaction newTransaction;
	
	newTransaction.accNumber = accountNum;
	newTransaction.amount = amount;
	newTransaction.transId = transactionIdCounter + 1;
	transactionIdCounter++;
	newTransaction.transType = 2;
	newTransaction.toAccount = NULL;
	newTransaction.transDate = getSystemDate();
			
	process(newTransaction);
}


/*
	Checks if the account has sufficient balance for withdrawal.
	@params
		amount - amount to be check if sufficient
		accountNum - account # of customer whose withdraw approval is to be checked
	@return 
		1 - Sufficient
		0 - Not sufficient
	
	@author Mark Torres 12/28/2018
	@modified AJ Ruth Sumandang 12/30/2018
		- (FROM)	if(amount < acc.balance){ return 1; } else if(amount == acc.balance){ return 1; }
		- (TO) 		if(amount <= acc.balance){ return 1; } 
		- Reason: Optimization
*/
int checkBalance(float amount,int accountNum){
	Account acc;
    char str[100];
    int counter = 1;
    
	acc = searchAccount(accountNum);
	if(amount <= acc.balance){
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
	Gets the system date and places it in a Date struct.
	@return 
		The current system date	
	
	@author Mark Torres 12/28/2018
	@modify AJ Ruth Sumandang (12/30/2018) 
		- (FROM)	struct tm *t = localtime(&now);
		- (TO)		struct tm t = localtime(&now); (and changed other parts accordingly)
		- Reason: No need for pointer. 
	@modify Cedric Alvaro 12/30/2018 
		- made it formal, to other used functions, just for formatting.
*/
Date getSystemDate(){
	time_t now = time(NULL);
   	struct tm t = *localtime(&now);
   	Date date;
   	date.day = t.tm_mday;
   	date.month = t.tm_mon + 1;
   	date.year = t.tm_year + 1900;
   	return date;
}

/*
	Process function writes the transactions to the file.
	@param 
		newTransaction - the transaction to be placed into the file
	
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
    strcat(filename, ".tr");
 
    fp = fopen(filename, "a");
    if (fp == NULL){
        printf("Could not open file %s", filename);
    } else{
    	fprintf(fp,"%d\n", newTransaction.transId);
    	fprintf(fp,"%d\n", newTransaction.transType);
    	fprintf(fp,"%d\n", newTransaction.accNumber);
    	fprintf(fp,"%d/%d/%d\n", newTransaction.transDate.month,newTransaction.transDate.day,newTransaction.transDate.year);
    	fprintf(fp,"%.2f\n", newTransaction.amount);
    	fprintf(fp,"%d\n", newTransaction.toAccount);
    	fprintf(fp,"\n");
	} 	
    fclose(fp);	
    postTransaction(newTransaction.accNumber);
    updateIds(getLatestIdFromStorage(2), newTransaction.transId);
}

/*
	Validates function accepts the PIN and check if it matches with the teller's PIN.
	@returns
		Value that tells caller if entered PIN is valid
		1 - if pin is valid
		0 - if pin is invalid
	
	@author Mark Torres (12/28/2018)	
*/
int validate(){
	int pin;
	printf(" \t\t==VALIDATION (Enter pin)==");
	pin = getUserInput();
	if(pin == tellerPin) {
		return 1;	
	} else{
		return 0;
	} 
}

/*
	Fetches the latest transaction number from a file.
	@returns
		The latest transaction ID
	@author Mark Torres (12/28/2018)	
*/
int getLatestTransactionId(){
	  return getLatestIdFromStorage(1);
}

/*
	Fetches the latest transaction number from a file.
	@params 
		type - the type of value to retrieve from file
			1 - Transaction
			2 - Account
	@returns
		The latest ID based on received type param
	
	@author Mark Torres (12/28/2018)
	@modified AJ Ruth Sumandang (12/31/2018)
		- Transferred code to this function
		- Reason: for function reusability
*/
int getLatestIdFromStorage(int type){
	FILE *fp;
    char str[100];
    char filename[100] = "Metadata.mt";
    int counter = 1;
 
    fp = fopen(filename, "r+");
    if(fp == NULL){
        printf("Could not open file %s", filename);
    } else{
    	while(fgets(str, 1000, fp) != NULL){
    		if(counter == type){
    			int latestId = atoi(str);
  				fclose(fp);	
				return latestId;
			}
			
  			counter++;
		}
		fclose(fp);	
	}
	return 0;
}

/*
	Writes the latest transactionId to a file.
	@params
		latestTransId - the latest transaction ID
		latestAccId - the latest account ID
	@author Mark Torres (12/28/2018)
	@modify AJ Ruth Sumandang (12/31/2018)
		- Change name from updateTransId -> updateIds
		- Reason: to accomodate change in storage; stored latest acc. # as well	
*/
void updateIds(int latestTransId, int latestAccId){
	FILE *fp;
    char filename[100] = "Metadata.mt";
 
    fp = fopen(filename, "w+");
    if (fp == NULL){
        printf("Could not open file %s", filename);
        return;
    } else{
    	fprintf(fp, "%d\n", latestTransId);
    	fprintf(fp, "%d", latestAccId);
    	fclose(fp);	
	}  
}

/*
	Calculates the new balance based on the total deposits, withdrawals and transfers.
	The data is retrieved from the transaction logs of the customer.
	@params
		accNumber - the account# of customer whose balance is to be calculated
		
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/
void postTransaction(int accNumber){
	Account accUpdated = getAccount(accNumber);
	printf(" \t\t==================================================\n");
	printf(" \tAccount Number: %d\n", accUpdated.accNumber);
	printf(" \tName: %s,%s %s\n", accUpdated.lastName, accUpdated.firstName, accUpdated.middleName);
	printf(" \t==================================================\n");
	printf(" \t			Summary						\n");
	printf(" \t==================================================\n");
	float totalDeposits = getTotal(2, accNumber);
		printf(" \tDeposits: %.2f\n", totalDeposits);
	float totalWithdrawals = getTotal(1, accNumber);
		printf(" \tWithdraws: %.2f\n", totalWithdrawals);
	float totalTransfers = getTotal(3,accNumber);
		printf(" \tTransfers: %.2f\n", totalTransfers);
	
	
	float newBalance = totalDeposits - (totalWithdrawals + totalTransfers) + getInterest(accUpdated);

	printf(" \t==================================================\n");	
	accUpdated.balance = newBalance;
	printf(" \tNew Balance: %.2f\n", accUpdated.balance);
	postAccountUpdate(accUpdated);
}

/*
	Gets the total deposits or withdrawals or transfers.
	
	@params
		transType - the transaction's type (1) (2) (3) (4)
		acctNumber - account # of customer whose transactions are to be calculated
	@return 
		total balance
		
	@author Mark Torres 12/28/2018
	@modify Mark Torres 12/28/2018	
*/

float getTotal(int transType,int acctNumber){
	FILE *fp;
    char *str = new char[1024];
    char accNumber[100];
    char filename[100] ;
    int counter = 1;
    Transaction transaction;
    float sum = 0;
    
    itoa(acctNumber, accNumber, 10);
    strcpy(filename, accNumber);
    strcat(filename, ".tr");
    
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s", filename);
        return 0;
    } else{
    	while (fgets(str, 100, fp) != NULL){
    		switch(counter){
    			case 1:
    				transaction.transId = atoi(str);
    				counter++;
					break;
				case 2:
					transaction.transType = atoi(str);
					counter++;
					break;
				case 3:
					transaction.accNumber = atoi(str);
					counter++;
					break;
				case 4:
					//DUMMY DATE (must call date transform function)
					/* TODO (#2#): Figure out what Mark means */
					transaction.transDate = getSystemDate();
					counter++;
					break;
				case 5:
					transaction.amount = atof(str);
					if(transaction.transType == transType){
						
						sum = sum + atof(str);
						counter++;
					}
					break;
				case 6:
					transaction.toAccount = atoi(str);
					counter = 0;
					break;
				case 0:
					counter = 1;		
					break;
			}	
		}
		fclose(fp);
		return sum;
	} 
}
/*
	Retrieves the Account specified by the account number.
	@params 
		accountNum - account # of account to be retrieved
	@returns 
		retrieved account
	
	@author Mark Torres (12/28/2018)
*/
Account getAccount(int accountNum){
	Account acc;
	
	FILE *fp;
    char str[100];
    char filename[100] = "account.in";
    int counter = 1;
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s", filename);
    } else{
    	acc = searchAccount(accountNum);
		fclose(fp);
	} 
	return acc; 
}                
