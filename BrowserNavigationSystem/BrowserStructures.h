#include <iostream>
#include <string>
#include <ctime>
using namespace std;

struct relatedPages {
	int r_id;
	string url;
	string title;
	relatedPages* next;
};

struct page {
	int p_id;
	string url;
	string title;
	time_t visitedAt;
	page* prev;
	page* next;
	relatedPages* rp_head;
};

struct p_history {
	page* head, * tail;
};

struct bookmarknode {
	page* p;
	bool favorite;
	int visitcount;
	time_t lastVisited;
	bookmarknode* prev, * next;
};

struct bookmarklist {
	bookmarknode* head, * tail;
};


struct Node {
	page* data;
	Node* next;
};

struct Queue {
	Node* front;
	Node* rear;
};





string deleteSpaces(string text);

void initialize(p_history& history, bookmarklist& bookmarks);

time_t parseDateTime(const string& dt);
void InsertDescendingBookmark(bookmarklist& bl, bookmarknode* b);
void readinputfile(const string& name, p_history& history,bookmarklist &bookmarks);

void InsertAtTail(p_history& h, page* p);
void InsertAtHead(page* p, relatedPages* curpage);


page* findPage(p_history& history, int newID);
bool RelatedPagesExists(relatedPages* head, int rID);
void addNewPage(p_history& history, page* p);

time_t convertTime(int day, int month, int year, int hour, int minute);
void DelOldHistory(p_history& h, time_t timegiven);

//function part 4
int NbofRelPages(page* currpage);
page* GetMostRelPage(p_history& h);

//my way in function part4 most related pages
bool isEmptyQueue(Queue &q);
void pushQueue(Queue &q, page* p);
page* popQueue(Queue &q);
int GetMaxRelNb(p_history& h);
Queue* GetMostRelPages(p_history &h);
void printQueue(Queue &q);

//function part5 del page
page* Exists(p_history& h, int pID);
void DelTarget(p_history& h, bookmarklist& b, page* target);

//function part 6 N recently bookmarks 
void PrintNRecentelyVisitedBookmarks(bookmarklist& b, int N);

//function part 7 search by substring
void FindPageBySubstring(p_history& h, string substring);
string ToLowerCase(string s);

//function part 8 Delete least visited bookmark
int MinimumVisitCount(bookmarklist& b);
void DelLeastVisitedBm(bookmarklist& b);

//function part 9 save to output file
string TimeToString(time_t t);
void PrintIntoOutputFile(p_history& h, bookmarklist& b);






//testing
void printRelatedPages(relatedPages* rp);
void printHistory(p_history& history);
void printBookmarks(bookmarklist& b);
