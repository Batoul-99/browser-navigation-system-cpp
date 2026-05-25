#include "BrowserStructures.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <direct.h>
#include <time.h>
using namespace std;

void initialize(p_history& history, bookmarklist& bookmarks) {
	history.head = NULL;
	history.tail = NULL;

	bookmarks.head = NULL;
	bookmarks.tail = NULL;
}


string deleteSpaces(string text) {
	if (text.empty()) return text;

	//First delete from beginning
	size_t startpos = text.find_first_not_of(' ');
	if (startpos == string::npos) return "";
	text.erase(0, startpos);

	//Now I want to delete spaces from end
	size_t endpos = text.find_last_not_of(' ');
	if (endpos == string::npos) return "";
	text.erase(endpos + 1);

	return text;
}


time_t parseDateTime(const string& dt){ //here I am converting time given(string) into time_t type 
	tm timeinfo = {};
	timeinfo.tm_isdst = -1;

	int day = stoi(dt.substr(0, 2));
	int month = stoi(dt.substr(3, 2));
	int year = stoi(dt.substr(6, 4));
	int hour = stoi(dt.substr(11, 2));
	int minute = stoi(dt.substr(14, 2));

	timeinfo.tm_mday = day;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_year = year - 1900;
	timeinfo.tm_hour = hour;
	timeinfo.tm_min = minute;
	timeinfo.tm_sec = 0;

	return mktime(&timeinfo);
}

void InsertDescendingBookmark(bookmarklist& bl, bookmarknode* b) {
	if (bl.head == NULL) {
		b->next = NULL;
		b->prev = NULL;
		bl.head = b;
		bl.tail = b;
		return;
	}

	if (b->lastVisited > bl.head->lastVisited) {
		b->next = bl.head;
		b->prev = NULL;
		bl.head->prev = b;
		bl.head = b;
		return;
	}
	
	bookmarknode* cur = bl.head;
	while (cur->next != NULL && cur->next->lastVisited > b->lastVisited)
		cur = cur->next;

	b->next = cur->next;
	b->prev = cur;

	if (cur->next != NULL)
		cur->next->prev = b;

	else
		bl.tail = b;

	cur->next = b;
}

void readinputfile(const string& name, p_history& history, bookmarklist& bookmarks) {
	cout << "Trying to open : " << name << endl;
	ifstream file(name);

	if (!file.is_open()) { //I am checking if the file opened successfully
		cout << "Can not open file " << name << " !" << endl;
		return;
	}

	page* currpage = NULL; //I created a pointer to the most recent page
	string line; // to hold each line in the file
	
	while (getline(file, line)) {
		if (line.empty()) continue; // here if a line is blank  I must skip it , go to the next line 

		string first = "", second = "", third = "", fourth = "";
		stringstream ss(line); //here I created it to treat each line in the original file as a mini file (easier in separating process)
		getline(ss, first, ',');
		getline(ss, second, ',');
		getline(ss, third, ',');
		getline(ss, fourth, ','); //even though the line ends without , it will save till the end of the line

		//now I want to clean the input parts from spaces , from the beginning and the tail
		second = deleteSpaces(second);
		third = deleteSpaces(third);
		fourth = deleteSpaces(fourth);
		

		char symbol = first[0];

		// first case - : New page 
		if (symbol == '-') {

			page* p = new page;
			//here I am filling data 
			p->p_id = stoi(first.substr(1));
			p->url = second;
			p->title = third;
			p->visitedAt = parseDateTime(fourth);

			//Now I must modify LINKS in DLL
			p->next = NULL;
			p->rp_head = NULL; // here still no related pages for it 

			if (history.head == NULL) {
				p->prev = NULL;
				history.head = p;
				history.tail = p;
			}

			else {
				p->prev = history.tail;
				history.tail->next = p;
				history.tail = p;
			}

			currpage = p;
		}
		
		// Second Case *: Related Page 
		else if (symbol == '*') {
			if (currpage == NULL) continue; //I can't make a page related to nothing!!

			relatedPages* r = new relatedPages;
			//Here I filled data
			r->r_id = stoi(first.substr(1));
			r->url = second;
			r->title = third;

			//Now I must modify LINKS
			r->next = currpage->rp_head; // why currpage ? because ALWAYS related pages are under the correct main page 
			currpage->rp_head = r; //I inserted it at the head of the Related Pages' List
		}

		//Now Third case #: Bookmark

		else if (symbol == '#') {
			bookmarknode* b = new bookmarknode;

			string isfavorite = first.substr(1); //yes || no  
			if (isfavorite == "yes") b->favorite = true;
			else b->favorite = false;

			b->visitcount = stoi(second); //here I convert the string to integer 
			b->lastVisited = parseDateTime(third);
			b->p = history.tail;      //ALWAYS it refers to last page visited , like in the input file 
			b->next = NULL;			  // he gave -101, ..,//google.com then #yes, 12.. last page was Google(p_id =101)
			
			InsertDescendingBookmark(bookmarks, b);
		}
	}



}

void InsertAtTail(p_history &h, page* p){
	p->next = NULL;
	if (h.head == NULL) {
		p->prev = NULL;
		h.head = p;
		h.tail = p;
	}
	else {
		p->prev = h.tail;
		h.tail->next = p;
		h.tail = p;
	}

}
void InsertAtHead(page* p, relatedPages* curpage) {
	curpage->next = p->rp_head;
	p->rp_head = curpage;
}
// AddNewPage (function2) allows user to open new page(like new tab in chrome) while program is running
//modify history

//here I am checking if the new page already exists using ID
page* findPage(p_history& history, int newID) {
	if (history.head == NULL) return NULL;
	page* cur = history.head;
	while (cur != NULL) {
		if (cur->p_id == newID)
			return cur;
		cur = cur->next;
	}
	return NULL;
}

//here also I must check if the related page gonna be added exists or not also using id

bool RelatedPagesExists(relatedPages* head, int rID) {
	if (head == NULL) return false;
	relatedPages* cur = head;
	while (cur != NULL) {
		if (cur->r_id == rID) return true;
		cur = cur->next;
	}
	return false;
}

void addNewPage(p_history &history,page* p) {

	InsertAtTail(history, p);
}

time_t convertTime(int day, int month, int year, int hour, int minute) {
	tm timeinfo = {};

	timeinfo.tm_mday = day;
	timeinfo.tm_mon = month - 1; //it starts from 0-11
	timeinfo.tm_year = year - 1900; //years since 1900
	timeinfo.tm_hour = hour;
	timeinfo.tm_min = minute;
	timeinfo.tm_sec = 0;

	return mktime(&timeinfo);
}


void DelOldHistory(p_history& h , time_t timegiven) {
	if (h.head == NULL) return;

	page* curr = h.head;

	while (curr != NULL) {
		if (curr->visitedAt < timegiven) {
			page* tmp = curr->next;
			relatedPages* itsRelPages = curr->rp_head;

			while (itsRelPages != NULL) {
				relatedPages* aux = itsRelPages;
				itsRelPages = itsRelPages->next;
				delete aux;
			}
			curr->rp_head = NULL;

			if (curr->prev == NULL) {
				h.head = curr->next;
				if (h.head != NULL)
					h.head->prev = NULL;
			}
			else if (curr->next == NULL) {
				h.tail = curr->prev;
				h.tail->next = NULL;
			}
			else {
				curr->prev->next = curr->next;
				curr->next->prev = curr->prev;
			}
			delete curr;
			curr = tmp;
		}
		else {
			curr = curr->next;
		}
	}
}


//Now function part4 (get most related page)

int NbofRelPages(page* currpage){
	if (currpage->rp_head == NULL) return 0;

	relatedPages* tmp = currpage->rp_head;
	int c = 0;

	while (tmp != NULL) {
		c++;
		tmp = tmp->next;
	}
	return c;
}



page* GetMostRelPage(p_history& h) {
	if (h.head == NULL) return NULL;

	page* mostrelpage =h.head;
	page* cur = h.head;
	int max= NbofRelPages(h.head);

	while (cur != NULL) {
		int count = NbofRelPages(cur);
		if (count > max) {
			max = count;
			mostrelpage = cur;
		}
		cur = cur->next;
	}
	return mostrelpage;
}


//But what if 2 or more pages have same exact nb of related pages ?

bool isEmptyQueue(Queue &q) {
	return(q.front == NULL);
}

void pushQueue(Queue &q, page* p) {
	Node* tmp = new Node;
	tmp->data = p;
	tmp->next = NULL;
	if (isEmptyQueue(q)) {
		q.front = tmp;
		q.rear = tmp;
		return;
	}
	q.rear->next = tmp;
	q.rear = tmp;
}

page* popQueue(Queue &q)
{
	if (isEmptyQueue(q)) {
		return NULL;
	}
	Node* tmp = q.front;
	page* relpage = tmp->data;

	q.front = q.front->next;
	delete tmp;

	return relpage;
}

int GetMaxRelNb(p_history& h) {
	if (h.head == NULL) return 0;

	
	page* cur = h.head->next;
	int max = NbofRelPages(h.head);

	while (cur != NULL) {
		int count = NbofRelPages(cur);
		if (count > max) {
			max = count;
		}
		cur = cur->next;
	}
	return max;
}

Queue* GetMostRelPages(p_history &h) {
	Queue* q = new Queue;
	q->front = NULL;
	q->rear = NULL;

	if (h.head == NULL) return q;

	int max = GetMaxRelNb(h);
	page* cur = h.head;

	while (cur != NULL) {
		int count = NbofRelPages(cur);
		if (count == max) {
			pushQueue(*q, cur);
		}
		cur = cur->next;
	}
	return q;
}

void printQueue(Queue &q) {
	if (q.front == NULL) {
		cout << "Queue is empty!" << endl;
		return;
	}

	Node* curr = q.front;
	cout << "\n***** Pages With Maximum Related Pages *****\n";

	while (curr != NULL) {
		page* p = curr->data;

		cout << "Page ID: " << p->p_id<< " | URL: " << p->url<< " | Title: " << p->title << " | Related Pages Count: " << NbofRelPages(p)<< endl;
			 
		curr = curr->next;
	}

	cout<<endl;
}

//Now part5 Delete page

page* Exists(p_history& h, int pID) {
	if (h.head == NULL) return NULL;

	page* tmp = h.head;
	while (tmp != NULL) {
		if (tmp->p_id == pID)
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

void DelTarget(p_history& h, bookmarklist& b, page* target) {

	if (target == NULL) return;

	relatedPages* aux = target->rp_head;
	while (aux != NULL) {
		relatedPages* todelete = aux;
		aux = aux->next;
		delete todelete;
	}

	target->rp_head = NULL;



	bookmarknode* curNode = b.head;
	while (curNode != NULL) {
		bookmarknode* tmp = curNode->next;

		if (curNode->p== target) {

			if (curNode->prev == NULL) {
				b.head = curNode->next;
				if (b.head != NULL)
					b.head->prev = NULL;
			}

			else if (curNode->next == NULL) {
				b.tail = curNode->prev;
				b.tail->next = NULL;
			}

			else {
				curNode->prev->next = curNode->next;
				curNode->next->prev = curNode->prev;
			}

			delete curNode;	
		}

		curNode = tmp;	
	}

		if (target->prev == NULL) {
				h.head = target->next;
				if (h.head != NULL)
					h.head->prev = NULL;
			}
		else if (target->next == NULL) {
				h.tail = target->prev;
				h.tail->next = NULL;
			}
		else {
				target->prev->next = target->next;
				target->next->prev = target->prev;
			}
		delete target;
}

//Function check N recent bookmarks O(N) , space complexity O(1)
void PrintNRecentelyVisitedBookmarks(bookmarklist& b, int N) {
	if (b.head == NULL) {
		cout << "No Bookmarks!" << endl;
		return;
	}
	int count = 0;

	bookmarknode* cur = b.head;
	while (cur != NULL && count < N) {
		cout << "Bookmark ID: " << cur->p->p_id << " | Favorite: " << (cur->favorite ? "Yes" : "No") << " | Visit Count: " << cur->visitcount << endl;
		cur = cur->next;
		count++;
	}
	cout << endl;
}

//Now part 7 Search by substring
string ToLowerCase(string s) {
	for (int i = 0; i < s.length(); i++)
		s[i] = tolower(s[i]);
	return s;
}

void FindPageBySubstring(p_history& h, string substring) {
	if (h.head == NULL) {
		cout << "History is Empty!" << endl;
		return;
	}

	page* cur = h.head;
	string searchURL;
	bool found = false;

	while (cur != NULL) {
		searchURL = ToLowerCase(cur->url);
		if (searchURL.find(substring) != string::npos) {
			if (found == false)
			cout << "\nPages that match this substring!" << endl;
			found = true;

			cout << "Page ID:" << cur->p_id << " | URL: " << cur->url << " |Title: " << cur->title << endl;
		
		}
		cur = cur->next;
	}
	cout << endl;
	if(!found)
	cout << "No Pages Match!" << endl;
	cout << endl;
}

//Now funnction part 8 Del Least Visited Bookmarks

int MinimumVisitCount(bookmarklist &b) {
	if (b.head == NULL) return 0;

	bookmarknode* bm = b.head;
	int min = b.head->visitcount;

	while (bm != NULL) {
		if (bm->visitcount <= min) {
			min = bm->visitcount;
		}
		bm = bm->next;
	}
	return min;
}

void DelLeastVisitedBm(bookmarklist& b) {
	if (b.head == NULL) {
		cout << "No Bookmarks Found!" << endl;
		return;
	}

	int minvisited = MinimumVisitCount(b);
	bookmarknode* cur = b.head;
	 
	while (cur != NULL) {
		bookmarknode* tmp = cur->next;

		if (cur->visitcount == minvisited) {
			if (cur->prev == NULL) {
				b.head = cur->next;
				if (cur->next != NULL)
					b.head->prev = NULL;
			}

			else if (cur->next == NULL) {
				b.tail = cur->prev;
				b.tail->next = NULL;
			}

			else {
				cur->prev->next = cur->next;
				cur->next->prev = cur->prev;
			}
			delete cur;
			cur = tmp;
		}

		else {
			cur = cur->next;
		}
	}
	cout << "\nLeast Visited Bookmark(s) Deleted Successfully!" << endl;
}

//Now funcction part 9 Save them into an output file
string TimeToString(time_t t){
	tm ti;
	localtime_s(&ti, &t);  

	char buffer[20];
	strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", &ti);
	return string(buffer);
}



void PrintIntoOutputFile(p_history& h, bookmarklist& b) {

	ofstream out("output.txt"); //here I did it automatically created by program 
								//also if the user choses this choice more than one time 
								// it will be overwritten!! (No duplicates)
	if (!out.is_open()) {
		cout << "\nCan't Create output.txt!\n";
		return;
	}

	page* cur = h.head;

	while (cur != NULL) {
		//PRINTING NOW PAGES , each page has related pages , and bookmarks

		out << "-" << cur->p_id << ", " << cur->url << ", " << cur->title << ", " << TimeToString(cur->visitedAt) << "\n";

		relatedPages* rp = cur->rp_head;
		while (rp != NULL) {
			out << "*" << rp->r_id << ", " << rp->url << ", " << rp->title << "\n";

			rp = rp->next;
		}

		bookmarknode* bm = b.head;
		while (bm != NULL) {
			if (bm->p == cur) {
				out << "#"
					<< (bm->favorite ? "yes" : "no") << ", "
					<< bm->visitcount << ", "
					<< TimeToString(bm->lastVisited) << "\n";
			}
			bm = bm->next;
		}
 
		cur = cur->next;
	}
	out.close();
	cout << "Data saved successfully to output.txt"<< endl;

}









// Testing!!
void printRelatedPages(relatedPages* rp) {
	while (rp != NULL) {
		cout << "     - Related ID: " << rp->r_id << " | URL: " << rp->url << " | Title: " << rp->title << endl;	
		rp = rp->next;
	}
}


void printHistory(p_history& history) {
	page* temp = history.head;

	cout << "\n========== Browsing History ==========\n";

	if (temp == NULL) {
		cout << "History is empty.\n";
		return;
	}

	while (temp != NULL) {
		cout << "Page ID: " << temp->p_id << " | URL: " << temp->url << " | Title: " << temp->title << endl;
		cout << "Related Pages:\n";
		printRelatedPages(temp->rp_head);

		cout << "\n";

		temp = temp->next;
	}
}

void printBookmarks(bookmarklist& b) {
	cout << "\n========== Bookmarks ==========\n";

	if (b.head == NULL) {
		cout << "\nNo Bookmarks Found.\n";
		return;
	}

	bookmarknode* cur = b.head;

	while (cur != NULL) {

		cout << "Bookmark -> Page ID: ";
		if (cur->p != NULL)
			cout << cur->p->p_id;
		else
			cout << "(DELETED PAGE)";

		cout << " | Favorite: " << (cur->favorite ? "Yes" : "No") << "  | Visit Count: " << cur->visitcount << "\n";
			
		cur = cur->next;
	}

	cout << "\n";
}




