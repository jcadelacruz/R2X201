#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

const int MAX_WIDTH = 10; //Maximum number of arrays to store
const int MAX_LENGTH = 4000; // Maximum number of lines to store
const int EDIT_LENGTH = 4;
const int MAX_PREV_LINE_SIZE = MAX_LENGTH;
const double RULER_MULT = 2;
string store[MAX_WIDTH][MAX_LENGTH];

class SearchTerm {
    public:
        // Attributes
        string name;
        string keyword;
        int offset;
        string edits[3];
    
        // Constructor
        SearchTerm(string k, int o, int e1, int e2) {
            name = k;
            keyword = k;
            offset = o;
            edits[1] = to_string(e1)+"";
            edits[2] = to_string(e2)+"";
        }
        SearchTerm(string n, string k, int o, int e1, int e2) {
            name = n;
            keyword = k;
            offset = o;
            edits[1] = to_string(e1)+"";
            edits[2] = to_string(e2)+"";
        }

    };

//misc
string produceRuler(int strl, bool isOnes){
    string ruler;
    if(isOnes){
        for(int j=1; j<strl*RULER_MULT; j++){
            ruler += to_string(j%10);
        }
    }
    else{
        for(int q=0; q<strl*RULER_MULT; q++){
            if(q%10==9) ruler += to_string(((q-(q%10))/10)+1);
            else ruler += " ";
        }
    }
    return ruler;
}
bool didCinFail(){
    
    if (cin.fail()) {
        cout<<"cin fail";
        cin.clear(); // Clear the error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
        cout << "Invalid input. Please enter 'Y' or 'N'." << endl;
        return true;
    }
    else{
        return false;
    }
}
bool isResponseYes(char response){
    if(response=='y'||response=='Y') return true;
    else if(response=='n'||response=='N') return false;
    else{
        cout << "\nInvalid input. Please enter 'Y' or 'N'." << endl;
        cin >> response;
        return isResponseYes(response);
    }
}
string replaceTabsWithSpaces(string str){
    string result = str;
    size_t pos = 0;
    while ((pos = result.find('\t', pos)) != string::npos){
        result.replace(pos,1," ");
        pos++;
    }
    return result;
}

//file managing
ifstream getFile(){
    string fileName;
    // Get file name from user
    cout << "Do NOT use spaces in responses\nEnter the file name (include .txt): ";
    cin >> fileName;
        //fileName = "NiO.txt";//TESTING
    // Open the file
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error opening file: " << fileName << endl;
        return getFile();
    }
    return file;
}
ifstream getFile(string fileName){
    cout << "Do NOT use spaces in responses";
    // Open the file
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error opening file: " << fileName << endl;
        return getFile();
    }
    return file;
}
//file reading
void setStringEdits(string str, string edits[]){
    // var initialization
    char response;
    int strl = str.length();
    string ruler=produceRuler(strl,true),rulerTens=produceRuler(strl,false);
    str = replaceTabsWithSpaces(str);

    // Ask user for edits
    cout << endl << str << "[END; not part of string]" << endl << ruler << endl << rulerTens; //display str
    cout << "\nDo you want to edit the string? Y/N ";
    cin >> response;
    if (!isResponseYes(response)){
        for(int i=0;i<EDIT_LENGTH;i++){ edits[i] = "0";}
    } else {
    // Logic to capture edits
        cout << "\nHow many chars trim off left side : ";
        cin >> edits[1];
        cout << "\nHow many chars trim off right side : ";
        cin >> edits[2];
    }
}
string editLine(string line, string edits[]){
    try {
        // Trim characters from the left
        int leftTrim = stoi(edits[1]);
        if (leftTrim < 0) {
            leftTrim = 0; // Ensure we don't trim a negative number
        }
        if (leftTrim > line.length()) {
            leftTrim = line.length(); // Ensure we don't trim more than the string length
        }
        line = line.substr(leftTrim); // Trim from the left

        // Trim characters from the right
        int rightTrim = stoi(edits[2]);
        if (rightTrim < 0) {
            rightTrim = 0; // Ensure we don't trim a negative number
        }
        if (rightTrim > line.length()) {
            rightTrim = line.length(); // Ensure we don't trim more than the string length
        }
        line = line.substr(0, line.length() - rightTrim); // Trim from the right

    } catch (const std::invalid_argument& e) {
        // Handle the error if stoi fails
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        // You may want to return the original line or handle it differently
    } catch (const std::out_of_range& e) {
        // Handle the case where the number is too large
        std::cerr << "Out of range: " << e.what() << std::endl;
    }

    return line; // Return the modified line
}
int searchAndStoreAndFindMaxLCount(ifstream &file, const string key, int storeLayer, int index){
    int i = index;
    string line;
    string edits[EDIT_LENGTH];
    string previousLines[MAX_LENGTH]; // Adjust size as needed
    int lineCount = 0; // To keep track of the number of lines read

    // Ask the user for the offset value
    int offset;
    cout << "Enter the number of lines to offset: ";
    cin >> offset;

    // Read file line by line
    while (getline(file, line) && index < MAX_LENGTH) {
        // Store the current line in the previousLines array
        previousLines[lineCount % MAX_PREV_LINE_SIZE] = line; // Use a fixed size for simplicity
        lineCount++;

        // Search for the keyword
        if (line.find(key) != string::npos) {
            // Check if we can use the offset line
            if (lineCount > offset) {
                // Use the line 'offset' lines above if available
                line = previousLines[(lineCount - offset - 1) % MAX_LENGTH];
            }

            // If first iteration, ask for edits on trimming
            if (i == index) {
                setStringEdits(line, edits);
            }
            line = editLine(line, edits); // Make edits

            store[storeLayer][index] = line; // Store the line
            index++;
        }
    }

    // Reset the file stream for the next call
    file.clear(); // Clear EOF flag
    file.seekg(0); // Move to the beginning of the file

    // Return max
    return index;
}
int searchAndStoreAndFindMaxLCount(ifstream &file, const string key, int storeLayer, int index, int offset, string edits[]){
    //cout << edits[0] << "   +  " << edits[1];
    int i = index;
    string line;
    string previousLines[MAX_LENGTH]; // Adjust size as needed
    int lineCount = 0; // To keep track of the number of lines read

    // Read file line by line
    while (getline(file, line) && index < MAX_LENGTH) {
        // Store the current line in the previousLines array
        previousLines[lineCount % MAX_PREV_LINE_SIZE] = line; // Use a fixed size for simplicity
        lineCount++;
        
        // Search for the keyword
        if (line.find(key) != string::npos) {
            //cout << "found";
            string k;
            //cin >> k;
            // Check if we can use the offset line
            if (lineCount > offset) {
                // Use the line 'offset' lines above if available
                line = previousLines[(lineCount - offset - 1) % MAX_LENGTH];
                //cout << "s";
                //cin >> k;
            }

            line = editLine(line, edits); // Make edits
            //cout << line;
            //cin >> k;

            store[storeLayer][index] = line; // Store the line
            //cout << "store";
            //cin >> k;
            index++;
        }
        
    }

    // Reset the file stream for the next call
    file.clear(); // Clear EOF flag
    file.seekg(0); // Move to the beginning of the file

    // Return max
    return index;
}
int searchAndStoreAndFindMaxLCount(ifstream &file, const string key, int storeLayer, int index, int offset, string edits[], int keycount, int keysize, string keyname, int maxLCount){
    //cout << edits[0] << "   +  " << edits[1];
    int i = index;
    string line;
    string previousLines[MAX_LENGTH]; // Adjust size as needed
    int lineCount = 0; // To keep track of the number of lines read

    // Read file line by line
    while (getline(file, line) && index < MAX_LENGTH) {
        // Store the current line in the previousLines array
        previousLines[lineCount % MAX_PREV_LINE_SIZE] = line; // Use a fixed size for simplicity
        lineCount++;
        
        // Search for the keyword
        if (line.find(key) != string::npos) {
            //cout << "found";
            string k;
            //cin >> k;
            // Check if we can use the offset line
            if (lineCount > offset) {
                // Use the line 'offset' lines above if available
                line = previousLines[(lineCount - offset - 1) % MAX_LENGTH];
                //cout << "s";
                //cin >> k;
            }

            line = editLine(line, edits); // Make edits
            //system("cls"); // For Windows
            cout << "\033[2J\033[1;1H" << endl << "["<< keycount <<"/"<<keysize<<"] "<< endl<<" checking '" << keyname << "' " << endl << lineCount/maxLCount << "%";
            //cout << line;
            //cin >> k;

            store[storeLayer][index] = line; // Store the line
            //cout << "store";
            //cin >> k;
            index++;
        }
        
    }

    // Reset the file stream for the next call
    file.clear(); // Clear EOF flag
    file.seekg(0); // Move to the beginning of the file

    // Return max
    return index;
}
//output
void linearSave(ofstream &outFile, int wCount, int maxLCount, string keyStore[]){
    // Output the stored lines to the file
    outFile << "Lines containing the keyword/s:" << endl;
    for (int q = 0; q <= wCount; q++) {
        if (q != wCount) outFile << "'" << keyStore[q] << "'\t";
        else outFile << "'" << keyStore[q] << "'\n";
    }
    // Print stored lines to the file
    for (int i = 0; i <= wCount; i++) {
        outFile << "KEYWORD "+keyStore[i] << endl;
        for (int p = 0; p < maxLCount; p++) {
            outFile << store[i][p] << endl;
        }
    }
}
void tabularSave(ofstream &outFile, int wCount, int maxLCount, string keyStore[]){
    // Print the stored key words to the file
    outFile << "Lines containing the keyword/s:" << endl;
    for (int q = 0; q <= wCount; q++) {
        if (q != wCount) outFile << keyStore[q] << "\t";
        else outFile << keyStore[q] << "\n";
    }
    // Print stored data to the file
    for (int p = 0; p <= maxLCount; p++) {
        for (int i = 0; i <= wCount; i++) {
            outFile << store[i][p] << '\t';
        }
        outFile << endl;
    }
}
void attemptSaveResultsToFile(const string& fileName, string keyStore[], int wCount, int maxLCount) {
    char response;
    cout << "\nDo you want to save the results to a file? Y/N: ";
    cin >> response;

    if (isResponseYes(response)) {
        ofstream outFile("results.txt"); // You can customize the file name as needed
        if (outFile.is_open()) {
            //determine save type
            cout << "\nSave linear or tabular. linear = Y/ tabular = N: ";
            cin >> response;

            if(isResponseYes(response)){
                linearSave(outFile, wCount, maxLCount, keyStore);
            }
            else{
                tabularSave(outFile, wCount, maxLCount, keyStore);
            }

            outFile.close();
            cout << "Results saved to 'results.txt'." << endl;
        } else {
            cerr << "Error opening file for writing." << endl;
        }
    } else {
        cout << "Results will not be saved." << endl;
    }
}
void coutputResults(int wCount, int maxLCount, string keyStore[]){
    // Output the stored lines
    cout << "\n\nLines containing the keyword/s:" << endl << "  ";
        // Print keywords
    for (int q = 0; q <= wCount; q++) {
        if(q!=wCount) cout << "'" << keyStore[q] << "',";
        else cout << "'" << keyStore[q] << "'\n";
    }
        // Print stored
    for (int i = 0; i <= wCount; i++) {
        for(int p = 0; p < maxLCount; p++){
            cout << store[i][p] << endl;
        }
    }
}

//presets
void runPreset(SearchTerm keys[], int keysize, int &wCount, int &maxLCount, std::string *keyStore, ifstream &file){
    int lCount;
    wCount--;
    for(int i=0; i<keysize; i++){
        wCount++;
        cout <<" ["<< i+1 <<"/"<< keysize <<"] checking '" << keys[i].name << "' offset " << keys[i].offset << endl;
        lCount = searchAndStoreAndFindMaxLCount(file, keys[i].keyword, wCount, 0, keys[i].offset, keys[i].edits);
        if(lCount>maxLCount) maxLCount = lCount;
        keyStore[wCount] = keys[i].name;
    }
}
void runPreset(SearchTerm keys[], int keysize, int &wCount, int &maxLCount, std::string *keyStore, ifstream &file, bool y){
    int lCount;
    wCount--;
    for(int i=0; i<keysize; i++){
        wCount++;
        cout <<" ["<< i+1 <<"/"<< keysize <<"] checking '" << keys[i].name << "' offset " << keys[i].offset << endl;
        lCount = searchAndStoreAndFindMaxLCount(file, keys[i].keyword, wCount, 0, keys[i].offset, keys[i].edits, i+1, keysize, keys[i].name, maxLCount);
        if(lCount>maxLCount) maxLCount = lCount;
        keyStore[wCount] = keys[i].name;
    }
}
    //scaps SearchTerms
    SearchTerm defFile("file:", 0, 17,0);
    SearchTerm affinity("affinity", 0, 38,0);
    SearchTerm hole("hole", 0, 38,0);
    SearchTerm voc("Voc", 0, 9,5);
    SearchTerm jsc("Jsc", 0, 9,7);
    SearchTerm ff("ff","eta", 1, 8,2);
    SearchTerm eta("eta", 0, 9,2);
    SearchTerm vmpp("V_MPP", 0, 11,5);
    SearchTerm jmpp("J_MPP", 0, 10,7);
    
    SearchTerm scapsKeys0[2]= {defFile, affinity};
    SearchTerm scapsKeys1[5]= {defFile, affinity, hole, voc, jsc};
    SearchTerm scapsKeys2[5]= {defFile, affinity, hole, ff, eta};
    SearchTerm scapsKeys3[5]= {defFile, affinity, hole, vmpp, jmpp};
    SearchTerm scapsKeysRes[7]= {defFile, affinity, hole, voc, jsc,ff, eta};
    SearchTerm scapsKeysFull[9]= {defFile, affinity, hole, voc, jsc,ff, eta, vmpp, jmpp};
    SearchTerm scapsKeys2nd[4]= {ff, eta, vmpp, jmpp};

int main() {
    ifstream file = getFile();
    string key;
    std::string keyStore[MAX_WIDTH];
    string line;
    int wCount = 0,maxLCount=1;
    bool reading = true;
    char response = 'y';
    int lCount;

    //Attempt to use preset search
    cout << endl << "Preset SCAPS search? y/n: ";
    cin >> response;
    if(isResponseYes(response)){
        cout << endl << "display? y/n: ";
        cin >> response;
        if(isResponseYes(response)){
            runPreset(scapsKeysFull,9,wCount, maxLCount, keyStore, file, true);
        }
        else{
            runPreset(scapsKeysFull,9,wCount, maxLCount, keyStore, file);
        }
    }
    else{
    while(reading){
        // Get keyword from user
        cout << endl << "Enter the keyword to search for: ";
        cin >> key;
            //key = "eta"; //TESTING

        // Search and Store
        cout << endl << "Confirm your keyword is '" << key << "' Y/N: ";
        cin >> response;
        if(isResponseYes(response)){
            lCount = searchAndStoreAndFindMaxLCount(file, key, wCount, 0);
            if(lCount>maxLCount) maxLCount = lCount;
            keyStore[wCount] = key;
        }
        else { continue;}

        // Ask user if done with reading
        cout << "Check another keyword? Y/N ";
        cin >> response;
        if (!isResponseYes(response)){ reading = false; continue;}

        wCount++;
    }}

    // Close the file
    file.close();

    //coutputResults(wCount, maxLCount, keyStore);//DISABLED
    attemptSaveResultsToFile("table of results", keyStore, wCount, maxLCount);

    
    cout << "End program [enter any key]";
    cin >> response;

    return 0;
}
