// Code for Project 1
// Report poll results

#include <iostream>
using namespace std;

int main()
{
    int numSurveyed;
    int numApprove;
    int numDisapprove;
    
    cout << "How many people were surveyed? ";
    cin >> numSurveyed;
    cout << "How many of them approve of the way the president is handling his job? ";
    cin >> numApprove;
    cout << "How many of them disapprove of the way the president is handling his job? ";
    cin >> numDisapprove;
    
    double pctApprove = numApprove / numSurveyed * 100.0;
    double pctDisapprove = numDisapprove / numSurveyed * 100.0;
    
    cout.setf(ios::fixed);
    cout.precision(1);
    
    cout << endl;
    cout << pctApprove << "% say they approve." << endl;
    cout << pctDisapprove << "% say they disapprove." << endl;
    
    if(numApprove > numDisapprove)
        cout << "More people approve than disapprove." << endl;
    else
        cout << "More people disapprove than approve." << endl;
}

