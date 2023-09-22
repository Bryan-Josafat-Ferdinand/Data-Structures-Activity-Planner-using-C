#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <time.h>

#define cls system("cls")
#define enter printf(">> ");

#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define BRED "\e[1;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define BBLU "\e[1;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define reset "\e[0m"

typedef struct {
	int day;
	int month;
	int year;
} dateDetail;

struct node {
	int activityID;
	char activityName[50];
	dateDetail date; 
    int rawDate; // hari + 30 * bulan + 365 * tahun
	int activityHour; 
	int activityMinute;
	char activityDesc[500];
	int activityPriority;
	int activityDone;
	int height;
	
	struct node *left;
	struct node *right;
};

int totalActivityCount = 0;

struct node *createNewNode(char name[50], dateDetail date, int hour, int minute, char desc[500], int priority) {
	struct node *newNode = (struct node *)malloc(sizeof(struct node));
	totalActivityCount++;

	newNode->activityID = totalActivityCount;
	strcpy(newNode->activityName, name); // nama aktivitas
    newNode->date.day = date.day; // hari aktivitas
	newNode->date.month = date.month; // bulan aktivitas
	newNode->date.year = date.year; // tahun aktivitas
    newNode->rawDate = date.day + (30 * date.month) + (365 * date.year);
	newNode->activityHour = hour; // jam aktivitas
	newNode->activityMinute = minute; // menit aktivitas
	strcpy(newNode->activityDesc, desc); // deskripsi aktivitas
	newNode->activityPriority = priority; // skala prioritas aktivitas
	newNode->activityDone = 0; // apakah aktivitas sudah selesai atau belum
	newNode->height = 0; //height awal aktivitas dalam AVL tree
	
	newNode->left = NULL;
	newNode->right = NULL;
	
	return newNode;
}

int max(int a, int b) {
	if(a > b) {
		return a;
	} else {
		return b;
	}
}

int height(struct node *root) {
	if(root == NULL) {
		return -1;
	} else {
		return root->height;
	}
}

struct node *rotateRight(struct node *root) {
	struct node *y = root->left;
	struct node *suby = y->right;
	
	y->right = root;
	root->left = suby;
	
	//update height
	root->height = 1 + max(height(root->left), height(root->right));
	y->height = 1 + max(height(y->left), height(y->right));
	
	return y;
}

struct node *rotateLeft(struct node *root) {
	struct node *x = root->right;
	struct node *subx = x->left;
	
	x->left = root;
	root->right = subx;
	
	//update height
	root->height = 1 + max(height(root->left), height(root->right));
	x->height = 1 + max(height(x->left), height(x->right));
	
	return x;
}


struct node *insertNode(struct node *root, char name[50], dateDetail date, int hour, int minute, char desc[500], int priority) {
    int rawDate = date.day + (30 * date.month) + (365 * date.year);
    
    if(root == NULL) {
        return createNewNode(name, date, hour, minute, desc, priority);
    } else if(rawDate >= root->rawDate) {
        root->right = insertNode(root->right, name, date, hour, minute, desc, priority);
    } else if(rawDate < root->rawDate) {
        root->left = insertNode(root->left, name, date, hour, minute, desc, priority);
    }
    
    // update height
	root->height = 1 + max(height(root->left), height(root->right));
	
	// balance
	//a. itung balance value
	int balance = getBalance(root);
	
	//b. cek berat kiri atau kanan
	//case 1: berat kiri
	if(balance > 1) {
		//c. rotasi untuk balance
		//case 1a: lurus
		if(rawDate < root->left->rawDate) {
			return rotateRight(root);
		}
		//case 1b: belok
		else if(rawDate >= root->left->rawDate) {
			root->left = rotateLeft(root->left);
			return rotateRight(root);
		}
	}
	//case 2: berat kanan
	else if(balance < -1) {
		//c. rotasi untuk balance
		//case 2a: lurus
		if(rawDate >= root->right->rawDate) {
			return rotateLeft(root);
		}
		//case 2b: belok
		else if(rawDate < root->right->rawDate) {
			root->right = rotateRight(root->right);
			return rotateLeft(root);
		}
	}
    return root;
}


void inOrder(struct node *root){ //inorder = Left Print Right
	if(root == NULL) {
		return;
	} else {
		inOrder(root->left);
		
		printf("========================================\n");
		printf("Activity #%d\n", root->activityID);
		printf("----------------\n");
		printf("Name: %s\n", root->activityName);
		printf("Date: %d-%d-%d\n", root->date.day, root->date.month, root->date.year);
		printf("Time: %d:%d\n", root->activityHour, root->activityMinute);
		printf("Description: %s\n", root->activityDesc);
		printf("Priority: ");
		if(root->activityPriority == 1) {
			printf("High\n");
		} else if(root->activityPriority == 2) {
			printf("Medium\n");
		} else if(root->activityPriority == 3) {
			printf("Low\n");
		}
		printf("Status: ");
		if(root->activityDone == 0) {
			printf("Incomplete\n");
		} else if(root->activityDone == 1) {
			printf("Complete\n");
		}
		printf("\n========================================\n");
		
		inOrder(root->right);
	}
}

void dateCheck(struct node *root, int currRD) {
	if(root == NULL) {
		return;
	} else if(currRD + 7 >= root->rawDate && currRD <= root->rawDate) {
		printf("Upcoming Activity #%d: %s\n", root->activityID, root->activityName);
		printf("\tDate: %d-%d-%d, Time: %d:%d\n\n", root->date.day, root->date.month, root->date.year, root->activityHour, root->activityMinute);
	}
	
	dateCheck(root->left, currRD);
	dateCheck(root->right, currRD);
}

struct node *updateActivity(struct node *root, int tempID) {
	int choice, temp;
	
	if(root == NULL) {
		return;
	} else if(root->activityID != tempID) {
		root->left = updateActivity(root->left, tempID);
		root->right = updateActivity(root->right, tempID);
	} else if(root->activityID == tempID) {
		do {
			cls;
			printf("=================\n");
			printf("Update Activity\n");
			printf("=================\n");
			
			printf("\n==========================================\n");
			printf("Activity #%d\n", root->activityID);
			printf("----------------\n");
			printf("Name: %s\n", root->activityName);
			printf("Date: %d-%d-%d\n", root->date.day, root->date.month, root->date.year);
			printf("Time: %d:%d\n", root->activityHour, root->activityMinute);
			printf("Description: %s\n", root->activityDesc);
			printf("Priority: ");
			if(root->activityPriority == 1) {
				printf("High\n");
			} else if(root->activityPriority == 2) {
				printf("Medium\n");
			} else if(root->activityPriority == 3) {
				printf("Low\n");
			}
			printf("Status: ");
			if(root->activityDone == 0) {
				printf("Incomplete\n");
			} else if(root->activityDone == 1) {
				printf("Complete\n");
			}
			printf("\n========================================\n");
			
			printf("\nUpdate which data?\n");
			printf("1. Name\n");
			printf("2. Time\n");
			printf("3. Description\n");
			printf("4. Priority\n");
			printf("5. Completion Status\n");
			printf("6. Return to main menu\n");
			printf(">> ");
			scanf("%d", &choice);
			
			switch(choice) {
				case 1:
					printf("\n\nEnter new name: ");
					fflush(stdin);
				    gets(root->activityName);
				    break;
				    
				case 2:
					printf("\n\nEnter new time (hh mm): ");
					scanf("%d %d", &root->activityHour, &root->activityMinute);
					getchar();
					break;
					
				case 3:
					printf("\n\nEnter new description: ");
					fflush(stdin);
				    gets(root->activityDesc);
				    break;
				    
				case 4:
					printf("\n\nEnter the new priority level:\n");
					printf("1. High\n");
					printf("2. Medium\n");
					printf("3. Low\n");
					printf(">> ");
					scanf("%d", &root->activityPriority);
					getchar();
					break;
					
				case 5:
					printf("\n\nIs the activity complete?\n");
					printf("1. Incomplete\n");
					printf("2. Complete\n");
					printf(">> ");
					scanf("%d", &temp);
					--temp;
					root->activityDone = temp;
					getchar();
					break;
					
				case 6:
					return root;
					break;
			}
			
			printf("\nActivity succesfully updated!\n");
			printf("Press Enter to continue.");
			getchar();
		} while(choice > 0 && choice < 7);
		
		return root;
	}
}

void exportActivityList(struct node *root) {
	FILE *fp = fopen("activity_list.csv", "a");
	
	if(root == NULL) {
		return;
	} else {
		exportActivityList(root->left);
		
		fprintf(fp, "%s,%d,%d,%d,%d,%d,%d,%d,%s\n", root->activityName, root->date.day, root->date.month, root->date.year, root->activityHour, root->activityMinute,
		root->activityPriority, root->activityDone, root->activityDesc);
		
		exportActivityList(root->right);
	}
	
	fclose(fp);
}

struct node *findMin(struct node *root){
    struct node* curr = root;
    while (curr && curr->left != NULL) {
        curr = curr->left;
    }
 
    return curr;
}

int getBalance(struct node *root) {
	if(root == NULL) {
		return 0;
	} else {
		return height(root->left) - height(root->right);
	}
}

struct node *removes(struct node *root, int num){
	if(root == NULL) {
		return root;
	} else if (num > root->rawDate) {
		root->right = removes(root->right, num);
	} else if (num < root->rawDate) {
		root->left = removes(root->left, num);
	} else { 
		if(root->left == NULL && root->right == NULL) {
			//struct node *temp = root;
			root = NULL;
			free(root);
			return root;
			
		} else if(root->right == NULL) { // 1 child kiri
			struct node *temp = root;
			root = root->left;
			free(temp);
			
		} else if(root->left == NULL) { // 1 child kanan
			struct node *temp = root;
			root = root->right;
			free(temp);
			
		} else { // 2 child
			struct node *minValue = findMin(root->right);
			
			root->activityID = minValue->activityID;
			strcpy(root->activityName, minValue->activityName);
			root->date.day = minValue->date.day;
			root->date.month = minValue->date.month;
			root->date.year = minValue->date.year;
			root->rawDate = minValue->rawDate;
			root->activityHour = minValue->activityHour;
			root->activityMinute = minValue->activityMinute;
			strcpy(root->activityDesc, minValue->activityDesc);
			root->activityPriority = minValue->activityPriority;
			root->activityDone = minValue->activityDone;
			
			root->right = removes(root->right, minValue->rawDate);
		}
		
	}
	
	// update height
	root->height = 1 + max(height(root->left), height(root->right));
	
	// balancing
	// get value
	int balance = getBalance(root);
	
	//case 1: berat kiri
	if(balance > 1) {
		//c. rotasi untuk balance
		//case 1a: lurus
		if(getBalance(root->left) >= 0) {
			return rotateRight(root);
		}
		//case 1b: belok
		else if(getBalance(root->left) < 0) {
			root->left = rotateLeft(root->left);
			return rotateRight(root);
		}
	}
	//case 2: berat kanan
	else if(balance < -1) {
		//c. rotasi untuk balance
		//case 2a: lurus
		if(getBalance(root->right) <= 0) {
			return rotateLeft(root);
		}
		//case 2b: belok
		else if(getBalance(root->right) > 0) {
			root->right = rotateRight(root->right);
			return rotateLeft(root);
		}
	}
	
	return root;
	
}

void findActivity(struct node *root, int tempID, struct node *first) {
	if(root == NULL) {
		return;
	} else if(root->activityID != tempID) {
		findActivity(root->left, tempID, first);
		findActivity(root->right, tempID, first);
	} else if(root->activityID == tempID) {
		removes(first, root->rawDate);
	}
}

void clearList(struct node *root) {
	if(root == NULL) {
		return;
	}	
	
	clearList(root->left);
	clearList(root->right);
		
	free(root);	
}

void loading_bar(int len, int time, char *message)
{
 char a = 177, b = 219;
 printf("\n[%s]\n", message);
    int i = 0;
 for (i = 0; i < len; i++)
  printf("%c", a);

 printf("\r");

    time *= 10;
 for (i = 0; i < len; i++) {
  printf("%c", b);
  Sleep(time);
 }
    puts("");
}

int main(){
	int i, j, choice, tempID, datefornow;
	struct node *root = NULL;
	struct node *first;
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	int currentDay = tm.tm_mday;
	int currentMonth = tm.tm_mon + 1;
	int currentYear = tm.tm_year + 1900;
	int currentRD = currentDay + (30 * currentMonth) + (365 * currentYear);
	
	printf("\n\n");
	loading_bar(50, 0, "Activity Planner");
	cls;
	
	do {
		printf("=========================================\n");
		printf(BBLU "ACTIVITY PLANNER\n" reset);
		printf("=========================================\n");
		printf(YEL "Current Date: %d-%d-%d\n", currentDay, currentMonth, currentYear);
		printf(reset "-----------------------------------------\n");
		printf(BRED "Reminder:\n");
		dateCheck(root, currentRD);
		printf(reset "=========================================\n\n");
		printf("Menu:\n");
		printf("1. Add new activity\n");
		printf("2. Check list of activity\n");
		printf("3. Update an activity\n");
		printf("4. Delete an activity\n");
        printf("5. Export activity list\n");
		printf("6. Exit program\n");
		printf("\n=========================================\n");
		enter scanf("%d", &choice);
		
		switch(choice) {
			case 1:
				cls;
                char tempName[50];
			    dateDetail tempDate;
			    int tempHour;
			    int tempMinute;
			    char tempDesc[500];
			    int tempPriority;
			    
			    printf("==================\n");
				printf("Add New Activity \n");
				printf("==================\n\n");
			
			    printf("- Activity name: ");
			    fflush(stdin);
			    gets(tempName);
			
			    printf("- Activity date (dd mm yyyy): ");
			    scanf("%d %d %d", &tempDate.day, &tempDate.month, &tempDate.year);
			
			    printf("- Time (hh mm): ");
			    scanf("%d %d", &tempHour, &tempMinute);
			
			    printf("- Activity Description: ");
			    fflush(stdin);
			    gets(tempDesc);
			
			    printf("- Priority Scale\n");
			    printf("1. High\n");
			    printf("2. Medium\n");
			    printf("3. Low\n");
			    enter scanf("%d", &tempPriority);
			    
			    printf("\nActivity has been successfully added!");
			    getchar();
			    
			    root = insertNode(root, tempName, tempDate, tempHour, tempMinute, tempDesc, tempPriority);
                printf("\n\nPress Enter to continue.\n");
				getchar();
				cls;
				break;
			
			case 2:
				cls;
			    inOrder(root);
			    getchar();
				printf("\n\nPress Enter to continue.\n");
				getchar();
				cls;
				break;
				
			case 3:
				cls;
				inOrder(root);
				if(root == NULL) {
					printf("There is no activity in the list!\n");
					getchar();
				} else {
					printf("\nWhich activity do you want to update?\n");
				printf("Enter ID: ");
				scanf("%d", &tempID);
				updateActivity(root, tempID);
				
				getchar();
				}

				printf("\n\nPress Enter to continue.\n");
				getchar();
				cls;
				break;
				
			case 4:
				cls;
				inOrder(root);
				printf("\nWhich activity do you want to delete?\n");
				printf("Enter ID: ");
				scanf("%d", &tempID);
				first = root;
				findActivity(root, tempID, first);
				
				getchar();
				printf("\n\nPress Enter to continue.\n");
				getchar();
				cls;
				break;
                
			case 5:
                cls;
                exportActivityList(root);
                getchar();
                printf("======================\n");
                printf("Export Activity List\n");
                printf("======================\n");
                
                printf("\nActivity list has been succesfully exported!");
                printf("\n\nPress Enter to continue.\n");
				getchar();
				cls;
                break;

			case 6:
				cls;
				printf("\n================================\n");
				printf("Thank you for using the program!\n");
				printf("================================\n");
				clearList(root);
				exit(420);
				break;
		}
	
	} while(choice > 0 && choice < 6);
	
	return 0;
}
