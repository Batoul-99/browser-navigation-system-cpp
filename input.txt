#include "BrowserStructures.h"
#include <iostream>
#include <string>
#include <filesystem>
using namespace std;


int main() {
	/*testing path
	cout << "DEBUG: CURRENT PATH = "
		<< std::filesystem::current_path()
		<< endl;
	*/

	p_history history;
	bookmarklist bookmarks;

	initialize(history, bookmarks);

	readinputfile("input.txt", history, bookmarks);

	//testing
	cout << "\nLoaded history from file:\n";
	printHistory(history);
	cout << "\n";
	cout << "\n printing bookmark list: \n";
	printBookmarks(bookmarks);
	cout << endl;
	

	int choice;

	while (true) {
		cout << "\n*********** Browser Navigation System ***********\n" << endl;
		cout << "1. Open A New Page \n";
		cout << "2. Delete Old history \n";
		cout << "3. Get Most Related Page \n";
		cout << "4. Delete A Page \n";
		cout << "5. View N most recent bookmarks \n";
		cout << "6. Search Pages By URL Substring \n";
		cout << "7. Remove Least Visited Bookmark \n";
		cout << "8. Save to Output File \n";
		cout << "9. Exit" << endl;
		int choice;

		while (true) {
			cout << "Please enter your choice (1-9): ";

			cin >> choice;

			if (choice >= 1 && choice <= 9)
				break;  // Valid input ? exit loop

			cout << "Invalid choice!Try again.\n";
			cin.clear();
			cin.ignore(10000, '\n');
		}

		switch (choice) {

		case 1:
		{
			int newID;
			cout << "Please enter the page ID :" << endl;
			cin >> newID;
			cin.ignore();

			page* exists = findPage(history, newID);
			page* newpage;

			if (exists != NULL) {
				cout << "This page already exists! Let's add the missing related pages.." << endl;
				newpage = exists;
			}

			else {
				newpage = new page;
				newpage->p_id = newID;

				cout << "Please enter URL :" << endl;
				getline(cin, newpage->url);


				cout << "Please enter title :\n";
				getline(cin, newpage->title);

				newpage->visitedAt = time(NULL);
				newpage->rp_head = NULL;

				addNewPage(history, newpage);
				cout << "Page Was Added Successfully!" << endl;
			}

			int nbrelatedpages;
			cout << "Please enter number of related pages :\n";
			cin >> nbrelatedpages;
			cin.ignore();

			for (int i = 0; i < nbrelatedpages; i++) {
				relatedPages* currelpage = new relatedPages;


				cout << "Enter ID for related page " << i + 1 << " :\n";
				cin >> currelpage->r_id;
				cin.ignore();

				cout << "Enter URL for related page " << i + 1 << " :\n";
				getline(cin, currelpage->url);

				cout << "Enter title for related page " << i + 1 << " :\n";
				getline(cin, currelpage->title);

				if (RelatedPagesExists(newpage->rp_head, currelpage->r_id) == false)
					InsertAtHead(newpage, currelpage);

				else {
					delete currelpage;
				}
			}

			cout << "Related Pages Added Successfully!" << endl;
			printHistory(history);
			printBookmarks(bookmarks);
			break;
		}

		case 2:
		{
			string date, time;
			string option;

			cin.ignore();

			cout << "Please enter the date (DD/MM/YYYY):" << endl;
			getline(cin, date);

			cout << "Enter time (HH:MM):" << endl;
			getline(cin, time);

			do {
				cout << "Are you sure that you want to remove the pages(and it's related pages) older than this date ?(yes/no)" << endl;
				getline(cin, option);
			} while (option != "yes" && option != "Yes" && option != "no" && option != "No");

			if (option == "No" || option == "no") break;

			int day = stoi(date.substr(0, 2));
			int month = stoi(date.substr(3, 2));
			int year = stoi(date.substr(6, 4));

			int hour = stoi(time.substr(0, 2));
			int minute = stoi(time.substr(3, 2));

			time_t timegiven = convertTime(day, month, year, hour, minute);

			DelOldHistory(history, timegiven);
			cout << "Old History Deleted!" << endl;
			printHistory(history);
			printBookmarks(bookmarks);
			break;
		}

		case 3:
		{	
			//Doctor requires this
			page* mostRelpage = GetMostRelPage(history);
			if (mostRelpage == NULL) {
				cout << "No pages found !" << endl;
				break;
			}
			cout << "Most Related Page :" << endl;
			cout << "Page ID: " << mostRelpage->p_id << " | URL: " << mostRelpage->url << " | Title :" << mostRelpage->title << endl;
			
			/* 
			//My way!
			Queue* Q = new Queue;
			Q->front = NULL;
			Q->rear = NULL;
			Q = GetMostRelPages(history);
			printQueue(*Q);
			*/
			break;
		}

		case 4:
		{
			int pID;

			cout << "Please enter the ID of the page you want to delete :" << endl;
			cin >> pID;

			page* target = Exists(history, pID);

			if (target == NULL) {
				cout << "This page doesn't exist!" << endl;
				break;
			}
			
			DelTarget(history, bookmarks, target);
			cout << "Page Deleted Successfully!" << endl;
			printHistory(history);
			printBookmarks(bookmarks);
			break;
		}

		case 5:
		{
			int N;
			do {
				cout << "Please enter number of recently visited bookmarks you want to check:" << endl;
				cin >> N;
			} while (N < 0);
			PrintNRecentelyVisitedBookmarks(bookmarks, N);
			break;
		}
		
		case 6:
		{
			string substring;
			cin.ignore();

			cout << "Please enter the substring :" << endl;
			getline(cin, substring);

			substring = deleteSpaces(substring);
			substring = ToLowerCase(substring);

			FindPageBySubstring(history, substring);
			break;
		}

		case 7:
		{
			cout << "\nDeleting Least Visited Bookmarks.." << endl;
			DelLeastVisitedBm(bookmarks);
			printHistory(history);
			printBookmarks(bookmarks);
			break;
		}

		case 8:
		{
			cout << "\nSaving to output.txt ..\n";
			PrintIntoOutputFile(history, bookmarks);
			break;
		}

		case 9:
		{
			cout << "\nThank You For Using My Browser Navigation System! " << endl;
			return 0;
		}


		}
	}
	return 0;
}