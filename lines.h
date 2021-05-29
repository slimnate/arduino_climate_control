#ifndef LINES_H
#define LINES_H

#define LINE_COUNT 12
#define LINE_SIZE  25

//class representing a list of strings (char [])
struct Lines {
    private:
        int _lineIndex = 0;
    public:
        char lines[LINE_COUNT][LINE_SIZE];
        int count();
        bool addLine(const char*);
        void getLine(char*, int);
        void printLines();
        
        static Lines split(const char*);
};

// return number of lines added to list
int Lines::count() {
    return _lineIndex + 1;
}

// copy 'line' into next empty line
bool Lines::addLine(const char* line) {
    if(strlen(line) > LINE_SIZE-1) {
        Serial.println("line too big");
        return false;
    }
    if(count() > LINE_COUNT){
        Serial.println("too many lines, not adding");
        return false;
    }
    Serial.println("added");
    strcpy(lines[_lineIndex], line);
    _lineIndex++;
}

// update 'dest' to value of line at 'index'
void Lines::getLine(char* dest, int index) {
    strcpy(dest, lines[index]);
}

// print line list to Serial
void Lines::printLines() {
    for(int i = 0; i < count(); i++) {
        char l[LINE_SIZE];
        getLine(l, i);
        Serial.println(l);
    }
}

// split a target string 'inStr' into lines and return new Lines object
Lines Lines::split(const char* inStr) {
    Serial.println("Splitting lines...");
    //vars
    Lines l;
    char lineBuff[LINE_SIZE];
    int bufferIndex = 0;
    int inLen = strlen(inStr);

    // clean line buffer before beginning
    memset(lineBuff, 0, LINE_SIZE);

    // loop through each char in input, adding lines as we go
    for(int i = 0; i <inLen; i++) {
        char c = inStr[i];
        if(c == '\r') {        // don't add to buffer
            //skip
        } else if(c == '\n') { // add line and reset buffer
            Serial.println("adding line: ");
            Serial.println(lineBuff);
            l.addLine(lineBuff);
            memset(lineBuff, 0, LINE_SIZE);
            bufferIndex = 0;
        } else {               // add char to buffer
            lineBuff[bufferIndex] = c;
            bufferIndex++;
        }
    }
    if(strlen(lineBuff) > 0) {
        // add anything left in the buffer as the final line, to handle no newline at end of str ing
        l.addLine(lineBuff);
    }

    return l;
}

#endif