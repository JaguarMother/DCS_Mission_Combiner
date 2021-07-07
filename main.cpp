
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include<iterator>
#include<regex>
/*
#include <cstdio>
#include<algorithm>
#include<sstream> //used to convert string to int
#include<cstdlib>
 */

using namespace std;

int const dcsCountryEnumCount = 85; //account for 0, so last num+1; https://wiki.hoggitworld.com/view/DCS_enum_country
string const countryList[dcsCountryEnumCount] = {"RUSSIA","UKRAINE","USA","TURKEY","UK","FRANCE","GERMANY","AGGRESSORS",
                                           "CANADA","SPAIN","THE_NETHERLANDS","BELGIUM","NORWAY","DENMARK","ISRAEL",
                                           "GEORGIA","INSURGENTS","ABKHAZIA","SOUTH_OSETIA","ITALY","AUSTRALIA",
                                           "SWITZERLAND","AUSTRIA","BELARUS","BULGARIA","CHEZH_REPUBLIC","CHINA",
                                           "CROATIA","EGYPT","FINLAND","GREECE","HUGARY","INDIA","IRAN","IRAQ",
                                           "JAPAN","KAZAKHSTAN","NORTH_KOREA","PAKISTAN","POLAND","ROMANIA",
                                           "SAUDI_ARABIA","SERBIA","SLOVAKIA","SOUTH_KOREA","SWEDEN","SYRIA",
                                           "YEMEN","VIETNAM","VENEZUELA","TUNISIA","THAILAND","SUDAN","PHILIPPINES",
                                           "MOROCCO","MEXICO","MALAYSIA","LIBYA","JORDAN","INDONESIA","HONDURAS",
                                           "ETHIOPIA","CHILE","BRAZIL","BAHRAIN","THIRDREICH","YUGOSLAVIA","USSR",
                                           "ITALIAN_SOCIAL_REPUBLIC","ALGERIA","KUWAIT","QATAR","OMAN",
                                           "UNITED_ARAB_EMIRATES","SOUTH_AFRICA","CUBA","PORTUGAL","GDR",
                                           "LEBANON","CJTF_BLUE","CJTF_RED","UN_PEACEKEEPERS","Argentina","Cyprus",
                                           "Slovenia"};

//Faction class used to store faction information from a file
class File{
public:
    File();
    explicit File(string& readFileAddress);
    int blueLine = 0; //holds any blue/red/neut/country line found during iteration
    int redLine = 0;
    int neutLine = 0;
    int countryLine = 0;

    int blueStart = 0;  //holds the actual start of unit list for each side
    int redStart = 0;
    int neutStart = 0;

    int blueEnd = 0;  //holds the actual ends of unit list for each side
    int redEnd = 0;
    int neutEnd = 0;

    //id list/points
    int idBlueCount = 0; //holds the total countries for blue in file
    int idRedCount = 0;
    int idNeutCount = 0;
    //type points

    string readFileAddress; //holds file address
    void findFactions();  //function that finds positions of paste point for each side (r/b/n)
    void processFile(); //goes through the mission file and finds, prints, and sets key mission file locations for rbn
    void buildCountryVector(vector<string> originalVector, int countryNum, vector<string>& builtVector); //returns a vector with countries listed in order for given faction
    void test();

    vector<string> blueText; //vector used to store all the lines for blue
    vector<string> redText; //vector used to store all the lines for red
    vector<string> neutText; //vector used to store all the lines for neutrals


    vector<string> blueFactionOrder;
    vector<string> redFactionOrder;
    vector<string> neutFactionOrder;
private:
};

File::File(){}
File::File(string& readFileAddress){
    this -> readFileAddress = readFileAddress;
    cout << endl << "---------- Searching " << this->readFileAddress << " ----------" << endl;
    findFactions(); //get rbn position lines at instantiation point
}

void File::findFactions(){

    fstream readFile(readFileAddress);  //open input file to grab string from

    int count=1; //counter used to track line number
    string line; //holds grabbed line

    cout << "LOCATING FACTION POSITIONS..." << endl;
    while(getline(readFile, line)){
        if (line.find("[\"name\"] = \"blue\"") != string::npos){
            //cout << "FOUND Blue start: " << count << endl ;
            blueLine = count;
        }
        if (line.find(R"(["name"] = "red")") != string::npos){
            //cout << "FOUND Red start: " << count << endl ;
            redLine = count;
        }
        if (line.find(R"(["name"] = "neutrals")") != string::npos){
            //cout << "FOUND Neutral start: " << count << endl ;
            neutLine = count;
        }
        if (line.find("[\"country\"] =") != string::npos){
            cout << "FOUND country line: " << count << endl ;
            countryLine = count;
            if (countryLine == blueLine + 1){ //check if country is right after blue, indicates correct position
                blueStart = blueLine;
                cout << "\t\t\tBlue group start at: " << blueStart << endl;
            }
            if (countryLine == redLine + 1){ //check if country is right after blue, indicates correct position
                redStart = redLine;
                cout << "\t\t\tRed group start at: " << redStart << endl;
            }
            if (countryLine == neutLine + 1){ //check if country is right after blue, indicates correct position
                neutStart = neutLine;
                cout << "\t\t\tNeutral group start at: " << neutStart << endl;
            }
        }
        if (line.find(R"(}, -- end of ["blue"])") != string::npos){
            cout << "FOUND Blue end: " << count << endl ;
            blueEnd = count;
        }
        if (line.find(R"(}, -- end of ["red"])") != string::npos){
            cout << "FOUND Red end: " << count << endl ;
            redEnd = count;
        }
        if (line.find(R"(}, -- end of ["neutrals"])") != string::npos){
            cout << "FOUND Neutral end: " << count << endl ;
            neutEnd = count;
        }
        count++;
    }
    readFile.close();
}

void File::processFile(){
    //copy lines to vector
    //process with changes
    //paste into file3

    fstream readFile(readFileAddress);  //open input file to grab string from
    int count = 1; //counter used to track line number
    string line; //holds grabbed line

    // read through file2 and copy applicable side lines into their vectors
    while(getline(readFile, line)) {
        if (count >= this->blueStart && count <= this->blueEnd) {
            blueText.push_back(line);
        }
        if (count >= this->redStart && count <= this->redEnd) {
            redText.push_back(line);
        }
        if (count >= this->neutStart && count <= this->neutEnd) {
            neutText.push_back(line);
        }
        count++;
    }

    cout << endl << endl << "\t\t\t\t\t\t\t" << readFileAddress;
    cout << endl << "\t\t\t\t\t\t\tBLUE Start: " << this->blueStart << " RED Start: " << this->redStart << " NEUT Start: " << this->neutStart;
    cout << "\t\t\t\t\t\t\t\t\tBLUE End: " << this->blueEnd << " RED End: " << this->redEnd << " NEUT End: " << this->neutEnd << endl;

    //get ID/country count in block to figure how many separate arrays are needed
    //mission file may vary in country counts

    //Get blue country count
    line = "";
    if(blueStart != 0) {
        for (int i = 0; i <= blueText.size(); i++) {
            //check if 1 country exists
            if (idBlueCount == 0 &&
                blueText[i].find(R"([1] = )") != string::npos) { //if any country exists, this will be true so at least 1
                idBlueCount = 1;
            }
            //check for last country and get max number
            if (idBlueCount > 0 && blueText[i].find(R"(-- end of ["country"])") != string::npos) {
                //cout << "Found more than 1 blue country" << endl;
                line = blueText[i -
                                1]; //copy line prior from country (contains the last country number) to string, used to extract number
                for (char x : line) {
                    if (isdigit(x)) {
                        idBlueCount = x - '0'; //convert the found char to int
                    }
                }
            }
        }
    }
    cout << "\t\t\t\t\t\t\tTotal blue countries found: " << idBlueCount << endl;
    line = "";
    //Check if red section exists and then get red country count
    if(redStart != 0) {
        for (int i = 0; i <= redText.size(); i++) {
            //check if 1 country exists
            if (idRedCount == 0 &&
                    redText[i].find(R"([1] = )") != string::npos) { //if any country exists, this will be true so at least 1
                idRedCount = 1;
            }
            //check for last country and get max number
            if (idRedCount > 0 && redText[i].find(R"(-- end of ["country"])") != string::npos) {
                //cout << "Found more than 1 blue country" << endl;
                line = redText[i -
                                1]; //copy line prior from country (contains the last country number) to string, used to extract number
                for (char x : line) {
                    if (isdigit(x)) {
                        idRedCount = x - '0'; //convert the found char to int
                    }
                }
            }
        }
    }
    cout << "\t\t\t\t\t\t\tTotal red countries found: " << idRedCount << endl;

/*    //Get neutral country count
    line = "";
    string xx;
    for (int i=0; i<= neutText.size(); i++){
        cout << "LINE: " << neutText[i] << endl;
        cout << "IDNEUT: " << idNeutCount << endl;
        if(regex_search(neutText[i], text)){
            cout <<"REGEX FOUND ITEM!" << endl;
        }
        //check if 1 country exists
        if (false){ //if any country exists, this will be true so at least 1
            idNeutCount = 1;
            cout <<"FOUND" << endl;
        }
        //check for last country and get max number
        if(idNeutCount > 0 && neutText[i].find(R"(-- end of ["country"])") != string::npos){
            //cout << "Found more than 1 blue country" << endl;
            line = neutText[i-1]; //copy line prior from country (contains the last country number) to string, used to extract number
            for (int x = 0; x < line.length(); x++){
                if (isdigit(line[x])){
                    idNeutCount = line[x] - '0'; //convert the found char to int
                }
            }
        }
    }
    cout << "\t\t\t\t\t\t\tTotal neutral countries found: " << idNeutCount << endl;*/
};

//string getString(istream& file, int num){
//readAddress = address for input file, targetLine = string to look for
int getLineNum(string readAddress, string searchString, int print){

    fstream readFile(readAddress);  //open input file to grab string from
    string line; //holds grabbed line
    int count = 0;  //counter for line number

    while(getline(readFile, line)){
        if (line.find(searchString) != string::npos){
            if(print != 0){
                cout << endl << "FOUND ON LINE: " << count+1 << endl ;
            }
            readFile.close();
            return count+1;
        }
        count ++;
    }
    readFile.close();
    return 0; //return 0 if no line found
}

//returns max id index from mission file
int getUnitMaxId(string readAddress){
    string targetText = "groupId";
    string line;
    int lineNum = 1; //counter to keep track of groupId found hits, indexes for array
    int id;
    vector<int> idVector; //vector to store all the line numbers for groupId hits
    vector<int> numList; //vector used to strip string down to integer, first is highest
    fstream readFile(readAddress);
    //stringstream stream(); //needed for grabbing number from string

    numList.push_back(0); //preset first vector element for comparison, without preset causes issue with comparison first iteration

    //while loop gathers all the groupId lines and stores line number in array
    while(getline(readFile, line)) {
        if (line.find(targetText) != string::npos) {
            idVector.push_back(lineNum); //add new element to vector that contains line number with groupId
        }
        lineNum++; //increment counter for each line passed
    }

    //loop through vector indexes and scan the file, extract and compare each id found, store highest groupID in first element of idVector
    //idVector holds line number containing groupId number
    //loop through file for each index and isolate each number comparing previous and keeping biggest
    for (int i = 0; i < idVector.size(); i++){
        fstream readFile(readAddress); //reopen file for each vector iteration
        for (int p = 0; p <= idVector[i]; p++){
            getline(readFile, line); //cycle through file and find line with groupId
            if (p == idVector[i] - 1){
                for (int a = 0; a < line.length(); a++){   //strip string down to number
                    if (isdigit(line[a])){
                        id = line[a] - '0'; //convert char to int
                        if(id > numList[0]) {
                            numList[0] = id;
                        }
                    }
                }
            }
        }
        readFile.close(); //close file for each vector iteration
    }
    return numList[0]; //return highest id value
}

string getString(const string &readAddress, int lineNum) {   //outputs string based on line number

    if (lineNum == 0) return ""; //if nothing found, don't return anything (0 is first line so nothing)
    fstream inputFile(readAddress);  //open input file to grab string from
    string line;   //holds grabbed string from input file
    for(int i = 0; i < lineNum + 1; i++){
        getline(inputFile, line);
        if (i == lineNum){
            inputFile.close();
            return line;
        }
    }
}

void createMission(string const& readAddress, string const& writeAddress){
    ifstream readFile1(readAddress);
    ofstream writeFile1(writeAddress);
    writeFile1 << readFile1.rdbuf(); //buffer provided object derived from basic_streambuf, get and set using rdbuf().
}

//countryNum = number of countries found in faction
void File::buildCountryVector(vector<string> originalVector, int countryNum, vector<string>& builtVector) {

    vector<string> textCopy = originalVector; //copy initial text block since check modifies original input for case insensitivity

    cout << countryNum << endl;//country count in blue
    //string line; //holds grabbed line
    int count = 0;  //counter for line number
    int matches = 0;  //index count to track how many found, part of condition for loop going through file

    //loop through lines in faction block, check for each country in array and if matched add to new vector that holds in order the found countries
    try{ //check for mismatch in searched countries and total to look for, catch possible memory error otherwise
        for (int i = 0; i <= textCopy.size() && matches < countryNum; i++) {
            for (int x = 0; x <= dcsCountryEnumCount - 1; x++) {
                transform(textCopy[i].begin(), textCopy[i].end(), textCopy[i].begin(), ::toupper);
                //cout << x << " SEARCHING: " << "[\"NAME\"] = \"" + countryList[x] << "    IN:" << textCopy[i] << endl;
                if (textCopy[i].find("[\"NAME\"] = \"" + countryList[x]) != string::npos) {
                    //cout << "\t\t\t FOUND: " << countryList[x] << endl;
                    matches++; //increment match count
                    builtVector.push_back(countryList[x]);
                    //cout << "matches: " << matches << endl;
                }
                if (x==dcsCountryEnumCount && matches < countryNum){
                    throw 1; //throw error if mismatch in found vs search total
                }
            }
        }
    } catch(int){
        cout << "\t\t\t\t\t\tERROR: MISMATCH IN COUNTRIES FOUND AND COUNTRIES SEARCHED.\n\t\t\t\t\t\tSEARCHED FOR " << countryNum << " FOUND " << matches << endl;
    }

    cout << matches << " COUNTRIES FOUND:" << endl;
    for (int i = 0; i<builtVector.size(); i++){
        cout << builtVector[i] << endl;
    }
    cout << endl;
}

//func to check if countries conflict, differs from checkCoalitionLists function - this one is a 'hard exit'/prog breaker vs just warning
int factionMismatchCheck(vector<string> vector1, vector<string> vector2) {
    for(int i = 0; i < vector1.size(); i++){
        for(int x = 0; x < vector2.size(); x++){
            if(vector1[i] == vector2[x]){
                cout << "\n\t\t\t\t\t\t\tERROR: MISMATCH IN FACTIONS FOUND. MAKE SURE NONE OF THE COUNTRIES ARE RED IN ONE MISSION AND BLUE IN ANOTHER.";
                system("pause"); //pause and wait for input
                exit(0);
            }
        }
    }
}

//checks for any difference in coalition lists, warns if different
void checkCoalitionLists(string readFileAddress1, string readFileAddress2){
    //fstream readFile1(readFileAddress1);  //open input file to grab string from
    //fstream readFile2(readFileAddress2);  //open input file to grab string from

    //variables that hold positions for all the start/end points
    int blueCoalitionStart;
    int redCoalitionStart;
    int neutCoalitionStart;
    int blueCoalitionEnd;
    int redCoalitionEnd;
    int neutCoalitionEnd;
    int count = 0; //counter used to track line number
    int startPosition = 0; //temp position track variable
    int endPosition = 0; //temp position track variable
    int addedCount = 0; //counter used to hold difference between the 2 main vectors
    //flags used to filter into correct vector when splitting
    bool neut = false;
    bool blue = false;
    bool red = false;
    string line; //holds grabbed line
    string filename;
    cout << "GETTING COALITION LIST POSITIONS..." << endl;
    //vectors hold the isolated lists of coalitions
    //main split into own sections
    vector<string> files{readFileAddress1, readFileAddress2};
    vector<string> main1;//used to store entire block of coalitions
    vector<string> main2;
    vector<string> blueList1;
    vector<string> redList1;
    vector<string> neutList1;
    vector<string> blueList2;
    vector<string> redList2;
    vector<string> neutList2;

    //copy coalition block to its vector, per file
    for(int a = 0; a < 2; a++){
        startPosition = getLineNum(files[a], R"(}, -- end of ["map"])", 0) + 2;
        endPosition = getLineNum(files[a], R"(}, -- end of ["coalitions"])", 0) - 2;
        cout << "\n\t" << files[a] << "\n\tCoalition list start: " << startPosition;
        cout << "\n\tCoalition list end: " << endPosition << endl;
        //open each file and copy block over to its vector based on found numbers earlier
        filename = "readFileAddress" + to_string(a+1);
        fstream file(filename);
        count = 0;
        fstream readFile(files[a]);  //open input file to grab string from

        while(getline(readFile, line)){
            if(count >= startPosition && count <= endPosition){
                if(a==0){
                    main1.push_back(line);
                    //cout << "MAIN1 COPY OUT: #" << count-startPosition << line << endl;
                }
                else if (a==1){
                    main2.push_back(line);
                    //cout << "MAIN2 COPY OUT: #" << count-startPosition << line << endl;
                }
            }
            count++;
        }
        file.close();
        cout << "main block " << a+1 <<" copied " << endl;
    }

    cout << "Precheck size1: " << main1.size();
    cout << endl << "Precheck size2: " << main2.size() << endl;
    //couldnt use vector of vectors, would keep going out of bounds of vector for some reason when tried accessing [i][x]
    //comparison used to determine range for the loops below, since files can differ in sizing of the coalition list
    //wrong size use can cause out of bounds error in looping through vector
    //check and use the smaller vector size for loop, in loop check for condition, if 0 and main2 is smaller, then add iterations

    //find whichever is bigger and match in size to avoid out of bound error with vector.size(),
    //equalizing vector size allows for the same loop counter max, again, a workaround to issue
    if(main1.size()>main2.size()){
        addedCount = main1.size() - main2.size();
        //cout << "added Count: " << addedCount << endl;
        for(int b = 0; b < addedCount; b++){
            main2.push_back(" ");
            //cout << "adding to main2..." << b << endl;
        }
    }
    else if(main2.size()>main1.size()){
        addedCount = main2.size() - main1.size();
        //cout << "added Count: " << addedCount << endl;
        for(int b = 0; b < addedCount; b++){
            main1.push_back("x");
            //cout << "adding to main1..." << b << endl;
        }
    }
    cout << "PostCheck size1: " << main1.size();
    cout << endl << "PostCheck size2: " << main2.size() << endl;
    for(int i = 0; i<2; i++){
        //break;
        for(int x=0;x < main1.size();x++){
            //cout << x << " ";
           if(main1.at(x).find(R"(["neutrals"] =)") != string::npos){
               //cout << "set neut" << endl;
               neut = true;
               red = false;
               blue = false;
           }
           if(main1.at(x).find(R"(["blue"] =)") != string::npos){
               //cout << "set blue" << endl;
               neut = false;
               red = false;
               blue = true;
           }
           if(main1.at(x).find(R"(["red"] =)") != string::npos){
               //cout << "set red" << endl;
               neut = false;
               red = true;
               blue = false;
           }
          if(neut){
               if(((main1.at(x).find(R"(-- end of ["neutrals"])") != string::npos) && i == 0) || ((main2.at(x).find(R"(-- end of ["neutrals"])") != string::npos) && i == 1)){
                   neut=false;
               }
               else if (i == 0){
                   //cout << "Copying to neutrals1: " << main1.at(x) << " ";// << endl;
                   neutList1.push_back(main1.at(i));
               }
               else if (i == 1){
                   //cout << "Copying to neutrals2: " << main2.at(x) << " ";// << endl;
                   neutList2.push_back(main2.at(i));
               }
           }
            if(blue){
                if(((main1.at(x).find(R"(-- end of ["blue"])") != string::npos) && i == 0) || ((main2.at(x).find(R"(-- end of ["blue"])") != string::npos) && i == 1)){
                    blue=false;
                }
                else if (i == 0){
                    //cout << "Copying to blue1: " << main1.at(x) << " ";// << endl;
                    blueList1.push_back(main1.at(i));
                }
                else if (i == 1){
                    //cout << "Copying to blue2: " << main2.at(x) << " ";// << endl;
                    blueList2.push_back(main2.at(i));
                }
           }
            if(red){
                if(((main1.at(x).find(R"(-- end of ["red"])") != string::npos) && i == 0) || ((main2.at(x).find(R"(-- end of ["red"])") != string::npos) && i == 1)){
                    red=false;
                }
                else if (i == 0){
                    //cout << "Copying to red1: " << main1.at(x) << " ";// << endl;
                    redList1.push_back(main1.at(x));
                }
                else if (i == 1){
                    //cout << "Copying to red2: " << main2.at(x) << " ";// << endl;
                    redList1.push_back(main2.at(x));
                }
            }
        }
    }

    //prep all the broken vectors for comparison, strip down each vector to its number only

    //for()
}

void File::test(){

}

int main() {

    string readFileAddress1 = "C:\\Users\\ocelo\\Desktop\\test1";
    string readFileAddress2 = "C:\\Users\\ocelo\\Desktop\\test2";
    string writeFileAddress = "C:\\Users\\ocelo\\Desktop\\test3";

    int num;
    int idIndex;
    int groupIndex;

    createMission(readFileAddress1, writeFileAddress);  //create base mission file by copying one of the missions

    File File1(readFileAddress1); //instantiate+start first search
    File File2(readFileAddress2);

    //process files and set rbn line numbers
    //divide file by rbn and add to their own vectors, also get count of countries in each faction
    cout << "\t\t\t\t\t\t\t\t\tFACTION POSITIONS:";
    File1.processFile();
    File2.processFile();

    //search for country id, get all the countries in missions per side, split and segragate, compare and paste
/*    File1.buildCountryVector(File1.blueText,2,File1.blueFactionOrder);
    File1.buildCountryVector(File1.redText,0,File1.redFactionOrder);*/
    cout << "BLUE COUNTRIES TO FIND IN FILE 1: ";
    File1.buildCountryVector(File1.blueText,File1.idBlueCount,File1.blueFactionOrder);
    cout << "RED COUNTRIES TO FIND IN FILE 1: ";
    File1.buildCountryVector(File1.redText,File1.idRedCount,File1.redFactionOrder);
                                                            /////////////neutral insert neutral vector build, also add neutral country check block in processFile
    cout << "BLUE COUNTRIES TO FIND IN FILE 2: ";
    File2.buildCountryVector(File2.blueText,File2.idBlueCount,File2.blueFactionOrder);
    cout << "RED COUNTRIES TO FIND IN FILE 2: ";
    File2.buildCountryVector(File2.redText,File2.idRedCount,File2.redFactionOrder);
                                                            /////////////neutral insert neutral vector build, also add neutral country check block in processFile

    //check if any country is blue in one mission and red in another, exit if so
    //File2.redFactionOrder.push_back("USA");
    factionMismatchCheck(File1.blueFactionOrder, File2.redFactionOrder);
    factionMismatchCheck(File2.blueFactionOrder, File1.redFactionOrder);
    cout << "\t\t\t\t\t\t\tMISMATCH CHECK - OK" << endl;

        //faction lists done, need to check which are shared with checkShared and then start combining the two missions
        //also check if groupid matters if it is in order, as in if mission 1 and 2 share USA, and mission 2 is copied in the groupid number is not in sequence

    checkCoalitionLists(readFileAddress1, readFileAddress2);

    File File3();
    return 0;
}


/* Drawbacks Current:
 * -no support for transfer of neutral units
 * -only air atm
 *
 * -implement a check and continue for mismatched id enums for coalition (separate from vectors made, this is an initial check for continuation, will use just one mission's set)
 *
 * -implement check for blue/red mismatch in faction, country on different sides in 2 missions
 *
 * -in order to maintain trigger retention and advanced waypoints, unit names are not changed, so each unit has to have a unique name
 * */

//some notes:
/*
 * check for blue/red spot relies on "[country]" being right after [blue] or [red] unit listing, currently that is the standard

*/
