/*b-stablo s brojevima*/
//ispis stabla te ispis inorder
//pretrazivanje el unutar stabla,br levela stabla,ispis najveceg i najmanjeg elementa

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXDEGREE 3//maksimalno kljuceva po cvoru

typedef struct Node* NodePointer;

typedef struct Node {
	int saved;// koliko je brojeva dosad u cvoru
	int keys[MAXDEGREE - 1];//kljucevi su svi brojevi u trenutnome cvoru
	NodePointer childrenPointer[MAXDEGREE];//pointer na sljedecu grane ako ovaj cvor nije list
}node;

int menu(NodePointer);
int console(NodePointer);
NodePointer createRootNode(int, NodePointer, NodePointer);
int print_inorder(NodePointer);
NodePointer Allocation();
NodePointer readFromFile(NodePointer);
NodePointer addNode(NodePointer);
int searchPosition(int, NodePointer, int);
int checkAndInsert(NodePointer, int, int*, NodePointer*);
int printTree(NodePointer, int);
NodePointer insert(int, NodePointer);
NodePointer userInsert(NodePointer);
NodePointer rightShift(NodePointer, int, int);
NodePointer createSplitNode(NodePointer*, NodePointer, int, NodePointer, int);
int minEl(NodePointer);
int maxEl(NodePointer);
int searchEl(NodePointer, int);
int printSearch(NodePointer);

int main() {
	NodePointer root = NULL;
	console(root);

	return 0;
}
int console(NodePointer root) {
	puts("Welcome to the menu!");
	puts("-----------------------------------------------------------");
	puts("Decide what you want to do with b-tree:");
	puts("Options are:");
	puts("read - It reads input from file and makes the tree");
	puts("insert - It inserts the key you want in the tree");
	puts("print - It prints the entire tree");
	puts("max - It prints the biggest element of the tree");
	puts("min - It prints the samllest element of the tree");
	puts("inorder - It prints numbers inside of the tree in order");
	puts("search - It enables you to search for a number in the tree");
	puts("end - It ends the whole program");
	puts("------------------------------------------------------------");
	puts("If you enter the wrong command you will be returned to the menu\n");
	menu(root);
	return 0;
};

int menu(NodePointer root) {
	char command[10];
	printf("command: ");
	scanf(" %[^\n]", command);
	if (!strcmp(command, "end")) {
		return 0;
	}
	else if (!strcmp(command, "read")) {
		root = readFromFile(root);
		menu(root);
	}
	else if (!strcmp(command, "insert")) {
		root = userInsert(root);
		menu(root);
	}
	else if (!strcmp(command, "search")) {
		printSearch(root);
		menu(root);
	}
	else if (!strcmp(command, "print")) {
		printf("Btree is :\n");
		printTree(root, 0);
	}
	else if (!strcmp(command, "min")) {
		int min = minEl(root);
		printf("Minimal element is :%d\n", min);
		menu(root);
	}
	else if (!strcmp(command, "max")) {
		int max = maxEl(root);
		printf("Maximal element is : %d\n", max);
		menu(root);
	}
	else if (!strcmp(command, "inorder")) {
		printf("Btree inorder is :\n");
		print_inorder(root);
		puts("\n");
		menu(root);
	}
	else {
		puts("\nYou've entered a wrong command, try again!\n");
		menu(root);
	}
	console(root);
	return 0;
}


NodePointer userInsert(NodePointer root) {
	//unos el preko konzole
	int number;

	printf("Insert the number you would like to add: ");
	scanf(" %d", &number);
	root = insert(number, root);

	return root;
}


NodePointer readFromFile(NodePointer root) {
	//unos el preko datoteke
	FILE *fp;
	int i = 0, numb = 0;
	char fileName[] = "numbers.txt ";

	fp = fopen(fileName, "r");
	if (!fp) {
		perror("Error in opening the file\n");
		return NULL;
	}
	while (!feof(fp)) {
		fscanf(fp, "%d", &numb);
		root = insert(numb, root);
	}
	fclose(fp);
	printf("\nThe numbers from file '%s' have been inserted\n\n",fileName);

	return root;
}


NodePointer insert(int key, NodePointer root) {
	//fja za unos
	NodePointer newNode;
	int topKey;
	int flag = checkAndInsert(root, key, &topKey, &newNode);;

	if (flag == -1)
		puts("Number is already inside of the tree");
	if (flag == 1) {
		root = createRootNode(topKey, root, newNode);
	}

	return root;
}


NodePointer createRootNode(int topKey, NodePointer prevRoot, NodePointer newNode) {
	//pozove se ili na pocetku izrade stabla ili kad je cvor pun pa se razdvoji na 3 cvora od kojih ovaj kreirani sadrzi samo lijevi srednji el
	NodePointer root = Allocation();
	root->saved = 1;
	root->keys[0] = topKey;
	root->childrenPointer[0] = prevRoot;
	root->childrenPointer[1] = newNode;

	return root;
}


NodePointer Allocation() {
	//alocira nove cvorove
	int i = 0;

	NodePointer btree = (NodePointer)malloc(sizeof(node));
	if (!btree) {
		perror("Node couldn't be allocated");
		return NULL;
	}
	for (i = 0; i < MAXDEGREE; i++) {
		btree->childrenPointer[i] = NULL;
	}
	btree->saved = 0;

	return btree;
}


int checkAndInsert(NodePointer root, int key, int* topKey, NodePointer* newNode) {
	//u fji se koristi vise pokazivaca kako bi vratili vise razlicitih podataka 

	//fja vraca 1 kad se el spremi u topKey a njegov desni pointer u newNode i treba ih dodati u root ili u neki novokreirani cvor
	//fja vraca 0 kad unutar cvora ima mjesta za ubacit el i taj el se ubaci
	//fja vraca -1 kad je el koji se unosi vec unutar stabla

	NodePointer newPtr, lastChild;
	int position, saved, splitPosition;
	int newKey, lastKey;
	int flag;

	if (root == NULL) {
		*newNode = NULL;
		*topKey = key;
		return 1;
	}

	saved = root->saved;
	position = searchPosition(key, root, saved);

	if (position < saved && key == root->keys[position])
		return -1;

	flag = checkAndInsert(root->childrenPointer[position], key, &newKey, &newPtr);

	if (flag != 1)
		return flag;

	if (saved < MAXDEGREE - 1) {
		position = searchPosition(newKey, root, saved);
		//na mjestu gdje treba ici novi element prethodno je spremljen drugi, zato pomak
		root = rightShift(root, saved, position);

		root->keys[position] = newKey;
		root->childrenPointer[position + 1] = newPtr;
		++root->saved;

		return 0;
	}

	//ako je cvor pun a novi bi po svojoj velicini trebao biti na kraju
	if (position == MAXDEGREE - 1) {
		lastKey = newKey;
		lastChild = newPtr;
	}
	else { //cvor pun,a novi ne ide na kraj,sve treba maknit u desno ali posto cemo zadnjeg izbacit trebamo ga zapamtit da ga nebi zauvijek izgubili
		lastKey = root->keys[MAXDEGREE - 2];
		lastChild = root->childrenPointer[MAXDEGREE - 1];

		root = rightShift(root, MAXDEGREE - 2, position);

		root->keys[position] = newKey;
		root->childrenPointer[position + 1] = newPtr;
	}
	//uzimamo srednji element, ako je paran br el. uzimamo srednji lijevi
	splitPosition = (MAXDEGREE - 1) / 2;
	(*topKey) = root->keys[splitPosition];
	root = createSplitNode(newNode, root, lastKey, lastChild, splitPosition + 1);

	return 1;
}


NodePointer createSplitNode(NodePointer* newNode, NodePointer root, int lastKey, NodePointer lastChild, int position) {
	//spremamo ono sto ce sadrzavat novi root cvor
	(*newNode) = Allocation();//Alociram cvor u koji cu spremit sve sto je prije bilo u cvoru prije dolaska novog el i to ce postati desno dijete novog cvora
	root->saved = position - 1; //br elemenata koji ce biti u lijevom dijetetu novog cvora
	int s=(*newNode)->saved = MAXDEGREE - position;//br elemenata koji ce bit u desnom dijetetu novog cvora

	for (int i = 0; i < s; i++) {
		(*newNode)->childrenPointer[i] = root->childrenPointer[i + position];
		if (i < s - 1)
			(*newNode)->keys[i] = root->keys[i + position];
		else
			(*newNode)->keys[i] = lastKey;
	}

	(*newNode)->childrenPointer[s] = lastChild;

	return root;
}

NodePointer rightShift(NodePointer b, int start, int position) {
	//pomak svi brojeva unutar cvora u desno za 1
	for (int i = start; i > position; i--) {
		b->keys[i] = b->keys[i - 1];
		b->childrenPointer[i + 1] = b->childrenPointer[i];
	}
	return b;
}

int searchPosition(int key, NodePointer search, int saved) {
	//pretraga pozicije gdje bi novi clan trebao doci
	int position = 0;

	while (position < saved && key >(search->keys[position]))
		position++;
	return position;
}

int printTree(NodePointer b, int blanks) {
	if (b) {
		int i;
		//iz estetskih razloga br razmaka se poveca kako idemo dublje u stablo
		for (i = 1; i <= blanks; i++)
			printf(" ");
		//takoder iz estetskih razloga zagrade koje obuhvacaju elemente koji su kljucevi jednog cvora
		printf("[");
		for (i = 0; i < b->saved; i++)
			printf(" %d ", b->keys[i]);
		printf("]");
		puts(" ");
		for (i = 0; i <= b->saved; i++) {
			printTree(b->childrenPointer[i], blanks + 12);
		}
	}
	return 0;
}


int print_inorder(NodePointer root) {
	//prvo lijevo dijete pa onda br iznad dijeteta pa desno dijete koje je ujedno lijevo sljed br. pa sljed. br. iznad pa njegovo desno pa opet br iznad...
	if (root) {
		int s = root->saved;
		int i = 0;
		if (s >= 1) {
			for (i; i <= s; i++) {
				print_inorder(root->childrenPointer[i]);
				if (i != s)
					printf("%d ", root->keys[i]);
			}
		}
	}

	return 0;
}

int minEl(NodePointer b) {
	//trazi krajnji lijevi jer je najmanji
	if (b) {
		int min;

		if (b->childrenPointer[0] != NULL)
			min = minEl(b->childrenPointer[0]);
		else
			min = b->keys[0];

		return min;
	}
	return 0;
}

int maxEl(NodePointer b) {
	//trazi krajnji desni jer je najveci
	if (b) {
		int max;
		int s = b->saved;

		if (b->childrenPointer[s] != NULL)
			max = maxEl(b->childrenPointer[s]);
		else
			max = b->keys[s - 1];

		return max;
	}

	return 0;
}

int printSearch(NodePointer b) {
	//korisnik upisuje koji el zeli naci
	int element;

	printf("Insert the element you are searching for: ");
	scanf("%d", &element);
	element = searchEl(b, element);

	return 0;
}

int searchEl(NodePointer b, int el) {
	//trazi el koji je korisnik zatrazio
	int i;
	if (b) {
		for (i = 0; i < b->saved; i++)
			if (b->keys[i] == el) {
				printf("There is element %d inside of the tree\n", el);
				return el;
			}
		for (i = 0; i <= b->saved; i++) {
			searchEl(b->childrenPointer[i], el);
		}
		return el;
	}
	return 0;
}


