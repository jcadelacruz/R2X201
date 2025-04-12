#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

const int MAX_WIDTH = 12; //Maximum number of arrays to store
const int MAX_LENGTH = 5000; // Maximum number of lines to store
const int EDIT_LENGTH = 4;
const int MAX_PREV_LINE_SIZE = MAX_LENGTH;
const double RULER_MULT = 2;
string store[MAX_WIDTH][MAX_LENGTH];

string TRIGGER = "Batch simulation # 1 step ";
string TRIGGER_RED = "Batch simulation #";

const int UNO = 143, DOS=385;
class SearchTerm {
    public:
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
        for(int j=1; j<strl*RULER_MULT; j++){ruler += to_string(j%10);}}
    else{
        for(int q=0; q<strl*RULER_MULT; q++){
            if(q%10==9) ruler += to_string(((q-(q%10))/10)+1); else ruler += " ";}}
    return ruler;
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
int countNonNumericFromRight(const std::string& str, bool skipFirstNumerical) {
    int count = 0;
    // Start from the end of the string and move backwards
    for (int i = str.length() - 1; i >= 0; --i) {
        if (!std::isdigit(str[i])) { // Check if the character is not a digit
            count++;
        } else {
            if(!skipFirstNumerical) break; // Stop counting when 2nd numerical character is encountered
            else skipFirstNumerical = false;
        }
    }
    return count;
}
int countUntilFromLeft(const std::string& str, const char delimiter){
    int count = 0;
    // Start from the end of the string and move backwards
    for (int i = 0; i < str.length(); i++) {
        if (str[i] != delimiter) { // Check if the character is delimiter
            count++;
        } else {
            break; // Stop counting when delimiter is encountered
        }
    }

    return count;
}
int countNumericFromLeft(const std::string& str){
	int count = 0;
    for (int i = 0; i < str.length(); i++) {
        if (std::isdigit(str[i])) { // Check if the character is not a digit
            count++;
        } else { break;}
    }

    return count;
}
string searchKey(const vector<string>& step, const string& key) {
    // Iterate through each line in the step
    for (const auto& line : step) { // Check if the line contains the key name
        if (line.find(key) != string::npos) {
            return line; // Return the line if the key is found
        }
    }
    return "notFound"; // Return "notFound" if the key is not found
}

//file managing
ifstream getFile(){
    string fileName;    // Get file name from user
    cout << "Do NOT use spaces in responses\nEnter the file name (include .txt): ";
    cin >> fileName;
    ifstream file(fileName); // Open the file
    if (!file.is_open()) {
        cout << "Error opening file: " << fileName << endl;
        return getFile();
    }
    return file;
}
//file reading
string editLine(string line, string edits[]){
    try {
        // Trim characters from the left
        int leftTrim = stoi(edits[1]);
        if (leftTrim < 0) { leftTrim = 0; cout << "leftTrim < 0";}
        line = line.substr(leftTrim); // Trim from left

        // Trim characters from the right
        int rightTrim = stoi(edits[2]);
            if(stoi(edits[2])==UNO){ rightTrim = countNonNumericFromRight(line);}//presets
            else if(stoi(edits[2])==DOS){ rightTrim = countNonNumericFromRight(line, false);}//skip first digit
        if (rightTrim < 0) { rightTrim = 0; cout << "rightTrim < 0";}
        line = line.substr(0, line.length() - rightTrim); // Trim from right

    } catch (const std::invalid_argument& e) { // case: stoi fails
        std::cerr << "Invalid argument: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) { // case: number is too large
        std::cerr << "Out of range: " << e.what() << std::endl;
    }

    return line; // Return the modified line
}
    //preset + per step
int searchAndStoreAndFindMaxLCountPerStep(ifstream &file, const string key, int storeLayer, int index, int offset, string edits[]){
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

//output
void saveStringVector(ofstream &outFile, vector<string> source){
    // Print the stored lines to the file
    for (string line : source) {
        outFile << line << "\n";
    }}
void tabularSave(ofstream &outFile, int wCount, int maxLCount, string keyStore[]){
    // Print the stored key words to the file
    outFile << "Lines containing the keyword/s:" << endl;
    for (int q = 0; q <= wCount; q++) {
        if (q != wCount) outFile << keyStore[q] << "\t";
        else outFile << keyStore[q] << "\n";
    }
    // Print stored data to the file
    for (int p = 0; p <= maxLCount; p++) {
        for (int i = 0; i <= wCount; i++) {  outFile << store[i][p] << '\t';}
        outFile << endl;
    }
}
void attemptSaveResultsToFile(string keyStore[], int wCount, int maxLCount) {
    string fileName;
    char response;
    cout << "\nDo you want to save the results to a file? Y/N: ";
    cin >> response;

    if (isResponseYes(response)) {
        cout << "\n Provide desired filename: ";
        cin >> filename;
        ofstream outFile(fileName);
        
        if (outFile.is_open()) {
            tabularSave(outFile, wCount, maxLCount, keyStore);
            
            outFile.close();
            cout << "\nResults saved to " << filename << endl;
        } else { cerr << "Error opening file for writing." << endl;}
    } else { cout << "Results will not be saved." << endl;}
}

//presets
vector<string> obtainStep(ifstream &file, int stepCount){
    int edit = TRIGGER.size();
    string edits[] = {"0", to_string(edit),"9"};
    trigger = TRIGGER_RED;
    
    vector<string> step;
    string line;
    bool withinStep = false;
    streampos lastPosition; // To store the position before the current step
    
    // Read the entire file 
    int q = 0;
    while (getline(file, line)) {
        if(line.find(trigger) != string::npos){
            if(withinStep){ break;}
            
            line = editLine(line, edits);
            
            if(stoi(line) == stepCount){ 
            	withinStep = true;
            	lastPosition = file.tellg();
            	//if(stepCount%100==1) cout << line << "v" << stepCount << endl;
            }
        }
        if(withinStep){
        	step.push_back(line);
        	}
    }
    
    // Reset the file stream for the next call
    file.seekg(lastPosition);
    file.seekg(-10000, ios::cur); 
    
    return step;
}
void runPresetPerStep(vector<SearchTerm> keys, int &wCount, int &maxLCount, string *keyStore, ifstream &file){
    int lCount;
    wCount = keys.size() +1;
    string line;
    
    // for each key
    for(int j=0; j<keys.size(); j++){ keyStore[j] = keys.at(j).name;}
    file.seekg(0);
    // for each step
    for(int s=0; s<MAX_LENGTH; s++){
        maxLCount = s+1;
        vector<string> step = obtainStep(file, s+1); //obtain step at s+1
        if(step.empty()) continue;
        for(int i=0; i<keysize; i++){
            line = "notFound";
            line = searchKey(step, keys[i].keyword);
            
            if(line!="notFound"){ store[i][s] = editLine(line, keys[i].edits);}
            else{ store[i][s] = "zarj";}
        }
        
        //max break
        if(s == 2503){
        	cout << "LKIT";
            break;
        }
        if(s==2503) break;
    }
}
void runPreset(SearchTerm keys[], int keysize, int &wCount, int &maxLCount, std::string *keyStore, ifstream &file){//the real one
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
    //scaps SearchTerms
    SearchTerm defFile("file:", 0, 17,1);
    SearchTerm affinity("affinity", 0, 38,UNO);
    SearchTerm hole("hole", 0, 38,UNO);
    
    SearchTerm voc("Voc", 0, 9,UNO);
    SearchTerm jsc("Jsc", 0, 9,DOS);
    SearchTerm ff("ff","eta", 1, 8,UNO);
    SearchTerm eta("eta", 0, 9,UNO);
    SearchTerm vmpp("V_MPP", 0, 11,UNO);
    SearchTerm jmpp("J_MPP", 0, 10,DOS);
    
    SearchTerm voc2("Voc", "Voc =", 0, 9,UNO);
    SearchTerm jsc2("Jsc", "Jsc =", 0, 9,DOS);
    SearchTerm ff2("ff","FF =", 1, 8,UNO);
    SearchTerm eta2("eta", "eta =", 0, 9,UNO);
    SearchTerm vmpp2("V_MPP", "V_MPP =", 0, 11,UNO);
    SearchTerm jmpp2("J_MPP", "J_MPP =", 0, 10,DOS);
    
    vector<SearchTerm> scapsKeys0 = {defFile, affinity};
    vector<SearchTerm> scapsKeys1 = {defFile, affinity, hole, voc, jsc};
    vector<SearchTerm> scapsKeys2 = {defFile, affinity, hole, ff, eta};
    vector<SearchTerm> scapsKeys3 = {defFile, affinity, hole, vmpp, jmpp};
    vector<SearchTerm> scapsKeysRes = {defFile, affinity, hole, voc, jsc,ff, eta};
    vector<SearchTerm> scapsKeysFull = {defFile, affinity, hole, voc, jsc,ff, eta, vmpp, jmpp};
    vector<SearchTerm> scapsKeysFull2 = {defFile, affinity, hole, voc2, jsc2,ff2, eta2, vmpp2, jmpp2};
    vector<SearchTerm> scapsKeys2nd = {ff, eta, vmpp, jmpp};

int main() {
    ifstream file = getFile();
    string key;
    std::string keyStore[MAX_WIDTH];
    string line;
    int wCount = 0,maxLCount=1;
    bool reading = true;
    
    cout << endl << "Preset SCAPS search";
    runPresetPerStep(scapsKeysFull2, wCount, maxLCount, keyStore, file);
    
    file.close();

    attemptSaveResultsToFile("table of results", keyStore, wCount, maxLCount);
    
    cout << "End program [enter any key]";
    cin >> response;

    return 0;
}
