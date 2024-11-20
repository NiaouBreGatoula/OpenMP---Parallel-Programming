#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_INT 2147483647 // max value of a integer can save
#define DEBUG 1        // some print messages for debugging and testing

typedef struct MatElem { // typedef για συντομογραφία
	int val;
	int i;
	int j;
} MatElem;

//functions
int isStrictDiagonalDominant(int const *const,int,int);
int absDiagonalMax(int const *const,int,int);
void calculateResultArray(int *, int const *const,int,int,int);
MatElem findMinReduction(int const *const,int n,int);
MatElem findMinCritical(int const *const,int n,int );
MatElem findMinBinaryTree(int const *const,int n,int );
void printMatrix(const int *,int,const char *);
void printDivider(int );                               
void printExecutionTimes(double,double,double,double,double,double);       
void GenerateStrictDiag(int *, int );
                                                                                                                                                    
               
// make 2d table to 1d                                                                                                                                   
#define INDEX(i, j, n) ((i) * (n) + (j))                            
int main(int argc, char *argv[])                                               
{                                                                           
    int *array, *resultArray;                                         
	int n;                                                             
	int t;
	int i, j;
	int inputChoice;
	int max;
	MatElem minMatElem_0,minMatElem_1,minMatElem_2;
	double startTime, endTime;
	double isStrictTime, absDiagonalMaxTime, calculateResultTime, findMinReductionTime, findMinCriticalTime, findMinBinaryTreeTime;

    printf("Enter the matrix size of N table: ");
    scanf("%d", &n);

    array = (int *)malloc(n * n * sizeof(int));
    resultArray = (int *)malloc(n * n * sizeof(int));
    printf("\nChoose matrix input method:\n"
           "1. Manually enter matrix values\n"
           "2. Use predefined strictly diagonally dominant matrix\nChoice: ");
    scanf("%d", &inputChoice);
    if (inputChoice == 1) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                printf("Enter value for A[%d][%d]: ", i, j);
                scanf("%d", &array[INDEX(i, j, n)]);
            }
        }
    } else if (inputChoice == 2) {
        GenerateStrictDiag(array,n);
    } else {
        printf("Wrong Input inputChoice");
        return 0;
    }
    printf("Threads Running: ");
    scanf("%d", &t);

   omp_set_num_threads(t);
    startTime = omp_get_wtime();
    if (!isStrictDiagonalDominant(array, n, t)) {
        printf("The matrix is not strictly diagonally dominant. Please try again!\n");
        free(array);
        free(resultArray);
        return 0;
    }
    endTime = omp_get_wtime();
    isStrictTime = endTime - startTime;

    printDivider(25);
    startTime = omp_get_wtime();
    max = absDiagonalMax(array, n, t);
    endTime = omp_get_wtime();
    absDiagonalMaxTime = endTime - startTime;

    if (DEBUG){ // Debug Mode On
        printMatrix(array, n, "Your Matrix A");
        printf("\nMaximum absolute diagonal value: %d\n", max);
        printDivider(25);
    }
    startTime = omp_get_wtime(); 
    calculateResultArray(resultArray, array, n, t, max);
    endTime = omp_get_wtime();
    calculateResultTime = endTime - startTime;

    startTime = omp_get_wtime();
    minMatElem_0 = findMinReduction(resultArray, n, t);
    endTime = omp_get_wtime();
    findMinReductionTime = endTime - startTime;

    startTime = omp_get_wtime();
    minMatElem_1 = findMinCritical(resultArray, n, t);
    endTime = omp_get_wtime();
    findMinCriticalTime = endTime - startTime;

    startTime = omp_get_wtime();
    minMatElem_2 = findMinBinaryTree(resultArray, n, t);
    endTime = omp_get_wtime();
    findMinBinaryTreeTime = endTime - startTime;

    printMatrix(resultArray, n, "Resultant Matrix B");
    printf("\nUsage of reduction clause -> Minimum value = %d, at position = %d, %d\n", minMatElem_0.val, minMatElem_0.i, minMatElem_0.j);
    printf("Usage of Critical area -> Minimum value = %d, at position = %d, %d\n", minMatElem_1.val, minMatElem_1.i, minMatElem_1.j);
    printf("Usage of Binary Tree clause -> Minimum value = %d, at position = %d, %d\n", minMatElem_2.val, minMatElem_2.i, minMatElem_2.j);
    printDivider(25);
    if (DEBUG)
         printExecutionTimes(isStrictTime, absDiagonalMaxTime, calculateResultTime, findMinReductionTime, findMinCriticalTime, findMinBinaryTreeTime);
    free(array);
    free(resultArray);
    return 0;
}


void printDivider(int length) 
{
    for (int i = 0; i < length; i++) {
        putchar('-');
    }
    putchar('\n');
}





void printMatrix(const int *matrix, int n, const char *title)  //print matrix's function
{ 
    printf("\n%s = {\n", title);
    for (int i = 0; i < n; i++) {
        printf("    {");
        for (int j = 0; j < n; j++) {
            printf("%d", matrix[INDEX(i, j, n)]);
            if (j < n - 1) {
                printf(", ");
            }
        }
        printf(" }");
        if (i < n - 1) {
            putchar(',');
        }
        putchar('\n');
    }
    printf("}\n");
}






int isStrictDiagonalDominant(int const *const array, int n, int threadsNum)  // check if it is Strict DiagonalDominant function
{
	int i, j;
	int lineSum;
	int flag = 1;
	int chunkSize = n / threadsNum;
	if (chunkSize == 0)
		chunkSize = 1;
	#pragma omp parallel shared(flag, chunkSize, n) private(i, j, lineSum)
	{
		#pragma omp for schedule(dynamic, chunkSize) // use dynamic because the operations are not the same for all threads (at some point all will continue)
		for (i = 0; i < n; i++)
		{
			if (!flag) 	// if a thread has found a line that breaks the strict diagonal dominance condition then skip the rest lines
				continue;
			lineSum = 0;
			for (j = 0; j < n; j++)
			{
				// printf("\nthread %d i %d j %d", omp_get_thread_num(), i, j);
				if (i != j)
					lineSum += abs(array[INDEX(i, j, n)]);
			}
			if (abs(array[INDEX(i, i, n)]) <= lineSum) 	// no need to lock because the final value will always be false even if 2+ threads try to write at the same time
				#pragma omp critical
                {
                    flag = 0;
                }
		}
	}
	return flag;
}





int absDiagonalMax(int const *const array, int n, int threadsNum)   // Find abs max from the diagonal values
{
	int i, j;
	int max, localMax;
	max = localMax = abs(array[0]);
	#pragma omp parallel for reduction(max: max) num_threads(threadsNum) //calculate max using reduction clause
	for (i = 0; i < n; i++)
	{
		if (abs(array[INDEX(i, i, n)]) > max)
			max = abs(array[INDEX(i, i, n)]);
	}
	return max;
}



void calculateResultArray(int *resultArray, int const *const array, int n, int threadsNum, int max) //Calculate B matrix [d.1]
{
	int i, j;
	int chunkSize = n / threadsNum;
	if (chunkSize == 0)
		chunkSize = 1;
	#pragma omp parallel  shared(n,  resultArray, chunkSize, max) private(i, j)
	{
		#pragma omp for schedule(static, chunkSize)
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				// printf("thread %d i=%d j=%d\n", omp_get_thread_num(), i , j);
				resultArray[INDEX(i, j, n)] = i == j ? max : (max - abs(array[INDEX(i, j, n)]));
			}
		}
	}
}


MatElem findMinReduction(const int *const array, int n, int threadsNum) {
    int minVal = MAX_INT; // Αρχικοποίηση με τον μέγιστο αριθμό int
    int minI = -1;        // Αρχικοποίηση εκτός φυσικής γραμμής πίνακα
    int minJ = -1;        // Αρχικοποίηση εκτός φυσικής στήλης πίνακα

    #pragma omp parallel for reduction(min:minVal) num_threads(threadsNum) //min is a build-in calculation of openmp reduction https://www.openmp.org/spec-html/5.0/openmpsu107.html
    for (int i = 0; i < n * n; i++) {
        int row = i / n;  // Υπολογισμός γραμμής
        int col = i % n;  // Υπολογισμός στήλης
        if (array[i] < minVal) {
            minVal = array[i];
            minI = row;
            minJ = col;
        }
    }
    // Δημιουργία και επιστροφή του αποτελέσματος
    MatElem globalMin;
    globalMin.val = minVal;
    globalMin.i = minI;
    globalMin.j = minJ;

    return globalMin;
}



MatElem findMinCritical(const int *const array, int n, int threadsNum) {
    MatElem min;
    min.val = MAX_INT; //Αρχικοποίηση τιμών ετσι ώστε να αρχικοποιήσοουμε τις local 
    min.i = -1;        
    min.j = -1;        

    
    int chunkSize = (n * n) / threadsNum; // Καθώς δεν γνωρίζουμε τον ακριβή αριθμό threads που θα μας δώσει ο χρ΄ηστης θα πρέπει να υπολογίσουμε εκείνη την στιγμή το σωστότερο διαμοίρασμα των εργασίων
    if (chunkSize == 0) {
        chunkSize = 1; // Αν υπάρχουν λιγότερα στοιχεία από threads, κάθε thread παίρνει τουλάχιστον 1 στοιχείο
    }

    #pragma omp parallel shared( n, chunkSize, min) // Ορίζουμε παράλληλο τμήμα
    {
      
        MatElem localMin;   // Δημιουργία τοπικων μεταβλητών 
        localMin.val = MAX_INT; // Αρχικοποίηση του τοπικού ελάχιστου στο μέγιστο δυνατό
        localMin.i = -1;        // Αρχικοποίηση της γραμμής εκτός πίνακα.
        localMin.j = -1;        // Αρχικοποίηση της στήλης εκτός πίνακα.

        #pragma omp for schedule(static, chunkSize)   // Το κάθε thread να επεξεργάζεται παράλληλα το τμήμα που του έχει ανατεθεί σύμφωνα με το chunkSize
        for (int i = 0; i < n * n; i++) {
            if (array[i] < localMin.val) { 
                localMin.val = array[i]; // Ενημέρωση τοπικής ελάχιστης τιμής 
                localMin.i = i / n;     // Ενημέρωση τοπικής γραμμής 
                localMin.j = i % n;     // Ενημέρωση τοπικής στήλης 
            }
        }
        #pragma omp critical   // Ενημέρωση της global ελάχιστης τιμής, προστατεύοντας το `min` από ταυτόχρονη πρόσβαση απο πολλούς threads(π.χ παράδειγμα με τουαλέτα :) )
        {
            if (localMin.val < min.val) {
                min.val = localMin.val; // Ενημέρωση της global ελάχιστης τιμής
                min.i = localMin.i;     // Ενημέρωση γραμμής
                min.j = localMin.j;     // Ενημέρωση στήλης
            }
        }
    }

    return min; 
}


MatElem findMinBinaryTree(const int *const array, int n, int threadsNum) {
    int totalSize = n*n;
    int chunkSize =totalSize/ threadsNum;
    if (chunkSize == 0) {
        chunkSize = 1;
    }
    MatElem *localMins = (MatElem *)malloc(threadsNum * sizeof(MatElem)); // Δημιουργία πίνακα για αποθήκευση τοπικών ελαχίστων ανά thread με δεσμευση μνήμης
    
    for (int i = 0; i < threadsNum; i++) { //Αρχικοποίηση όλων των τιμών (διαδικό δέντρο)
        localMins[i].val = MAX_INT;
        localMins[i].i = -1;
        localMins[i].j = -1;
    }


    #pragma omp parallel shared( localMins, n, chunkSize) // Ορίζουμε παράλληλο τμήμα
    {
        int threadId = omp_get_thread_num(); // Λήψη του ID του τρέχοντος thread
        int startIndex = threadId * chunkSize; // Υπολογισμός αρχικού index για το thread
        int endIndex = startIndex + chunkSize;   // Υπολογισμός τελικού index για το thread
        if (endIndex > totalSize) {
            endIndex = totalSize; //έχει ξεφύγει απο τον πίνακα απλα πάρε το υπόλοιπο..
        }

        for (int i = startIndex; i < endIndex; i++) { // Με βάση τα προηγούμενα βρίσκουμε το min στον πίνακα με τα στοιχεία που βρήκαμε
            if (array[i] < localMins[threadId].val) {
                localMins[threadId].val = array[i]; // αποθήκευση value
                localMins[threadId].i = i / n;   // αποθήκευση value γραμμής
                localMins[threadId].j = i % n;   // αποθήκευση value στήλης
            }
        }
    }

    // -------------------- binary tree reduction----------------------
    int activeThreads = threadsNum; 
    while (activeThreads > 1) {
        int half = (activeThreads + 1) / 2; // Χωρίζουμε σε ζευγάρια
        
        #pragma omp parallel for
            for (int i = 0; i < half; i++) {
                if (i + half < activeThreads && localMins[i + half].val < localMins[i].val) {
                    localMins[i] = localMins[i + half];  // Αν το ελάχιστο του άλλου ζευγαριού είναι μικρότερο ενημερώνουμε το ελάχιστο
                }
            }
        activeThreads = half; 
    }

    MatElem min = localMins[0]; //Πρώτο στοιχείο.. αφου έχει μείνει ενα στοιχείο στο πίνακα οπού είναι και το ελάχιστο
    free(localMins);         

    return min; 
}


void printExecutionTimes(double isStrictTime, double absDiagonalMaxTime, double calculateResultTime, double findMinReductionTime, double findMinCriticalTime, double findMinBinaryTreeTime) {
    printf("\nExecution Times:\n");
    printf("1. Time taken for isStrictDiagonalDominant: %f seconds\n", isStrictTime);
    printf("2. Time taken for absDiagonalMax: %f seconds\n", absDiagonalMaxTime);
    printf("3. Time taken for calculateResultArray: %f seconds\n", calculateResultTime);
    printf("4. Time taken for findMinReduction: %f seconds\n", findMinReductionTime);
    printf("5. Time taken for findMinCritical: %f seconds\n", findMinCriticalTime);
    printf("6. Time taken for findMinBinaryTree: %f seconds\n", findMinBinaryTreeTime);
}


void GenerateStrictDiag(int *array, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                array[INDEX(i, j, n)] = rand() % 10; // Τυχαία τιμή από 0 έως 9
                sum += abs(array[INDEX(i, j, n)]);  
            }
        }
        array[INDEX(i, i, n)] = sum + 1;
    }
}




