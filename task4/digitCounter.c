int digitCount(const char* input){
    int counter = 0;
    int i =0;
    while (input[i] != '\0') {
        int asciiRep = input[i] - '0';
        if(asciiRep >= 0 && asciiRep <= 9){
            counter++;
        }
        i++;
    }
    return counter;
}

int main(int argc, char** argv){
    return 0;
}

