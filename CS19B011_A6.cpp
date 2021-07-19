#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

int cacheSize=0;
int blockSize=0;
int assoc=0;
int replace=0;

int noOfBlocks=0;
int sets=0;
int setSize=0;

int tagBits=0;
int indexBits=0;
int offSet=0;


int cacheAccesses=0;
int readAccesses=0;
int writeAccesses=0;

int cacheMisses=0;
int compMisses=0;
int capMisses=0;
int confMisses=0;

int readMisses=0;
int writeMisses=0;
int dirtyBlocksEvic=0;n


bool* convertHexToBinary(string s){
    bool* binaryForm = new bool[32];

    int i=0;
    int j=0;

    while(i<32){

        int digit = 0;
        if(s[j] >= '0' && s[j] <= '9') digit = ((int)s[j]) - 48;
        if(s[j] >= 'A' && s[j] <= 'F') digit = ((int)s[j]) - 65 + 10;
        if(s[j] >= 'a' && s[j] <= 'f') digit = ((int)s[j]) - 97 + 10;

        for(int k = 3; k>=0; k--){
            binaryForm[i+k] = digit%2;
            digit = digit/2;
        }

        i+=4;
        j++;
    }

    return binaryForm;
}

int convertBinaryToInt(bool *b, int low; int high){
    int integer=0;
    for(int i=high; i>=low; i--) integer += b[i]*2;
    return integer;
}



bool binarySearch(int *A, int size, int n){
    if(size==0) return 0;

    int low = 0;
    int high = num-1;

    while(low<=high){
        int mid = low + (high - low)/2;
        if(A[mid] == n) return 1;
        if(A[mid] > n) high = mid-1;
        if(A[mid] < n) low = mid+1; 
    }

    return 0;
}

int* insertElement(int *A, int size, int n){
    int* B = new int[size+1];\
    bool inserted=0;

    for(int i=0; i<size+1; i++){
        if(!inserted && n<=A[i]){
            inserted = 1;
            B[i] = n;
        }

        else B[i] = A[i - inserted];
    } 

    return B;
}




struct cacheBlock{
    int tag;
    int validBit;
    int dirtyBit;
    struct cacheBlock* next;
};


bool searchS0(struct cacheBlock* A, int T){
    struct cacheBlock* temp = A->next;
    while(temp != NULL){
        if(temp->tag == T && temp->validBit==1) return 1;
        temp = temp->next;
    }

    return 0;
}

void LRU1(struct cacheBlock* A, int T, int request){
    struct cacheBlock* temp = A->next;
    struct cacheBlock* prev = A;
    while(temp != NULL){
        if(temp->tag == T){
            struct cacheBlock* S = new struct cacheBlock;
            S->tag = T;
            S->validBit = 1;
            S->dirtyBit = temp->dirtyBit;
            S->next = NULL;

            if(S->dirtyBit == 1) dirtyBlocksEvic++;
            if(request == 1) S->dirtyBit = 1;
            
            if(prev == A){
                A->next = A->next->next;
                S->next = A->next;
                A->next = S;
            }

            else{
                prev->next = temp->next;
                S->next = A->next;
                A->next = S;
            }

            break;
        }

        prev=temp;
        temp=temp->next;
    }
}


void insertCacheBlock(int place, struct cacheBlock* A, int T){

    struct cacheBlock* temp = A;
    int i=0;

    struct cacheBlock* S = new struct cacheBlock;
    S->tag = tagS;
    S->validBit = 1;
    S->dirtyBit = 0;
    S->next=NULL;

    if(place == 0){
        S->next = A->next;
        A->next = S;
    }

    else{    
        while(temp->next!=NULL && i<place){
            temp=temp->next;
            i++;
        }

        S->next = temp->next;
        temp->next = S->next;
    }
}





int main(){

    cin >> cacheSize >> blockSize >> assoc >> replace;

    noOfBlocks = cacheSize/blockSize;

    sets = 1;
    setSize = noOfBlocks;

    if(assoc == 1){
        sets = noOfBlocks;
        setSize = 1;
    }

    if(assoc > 1){
        sets = noOfBlocks/assoc;
        setSize = assoc;
    }

    indexBits = log2(sets);
    offSet = log2(blockSize);
    
    tagBits = 32 - (indexBits + offSet);

    struct cacheBlock** Cache = new struct cacheBlock*[sets]; 
    
    for(int i=0; i<sets; i++){
        Cache[i] =  new struct cacheBlock;
        Cache[i]->tag = 0;
        Cache[i]->validBit = 0;
        Cache[i]->dirtyBit = 0;
        Cache[i]->next = NULL;
    }

    bool** trees;
    if(replace == 2){
        trees = new bool*[sets];
        for(int i=0; i<sets; i++){
            trees[i] = new bool[2*setSize - 1];
            for(int j=0; j<2*setSize-1; j++) trees[i][j] = 0;
        }

        
        for(int i=0; i<sets; i++){
            struct cacheBlock* SetPLRU = Cache[i];
            for(int j=0; j<setSize; j++){
                SetPLRU->next = new struct cacheBlock;

                SetPLRU->next->tag = 0;
                SetPLRU->next]->validBit = 0;
                SetPLRU->next->dirtyBit = 0;
                SetPLRU->next->next = NULL; 

                SetPLRU = SetPLRU->next;               
            }
        }
    }

    int dataSize=0;
    int *data = new int[dataSize];
    
    int *valids = new int[sets];

    for(int i=0; i<sets; i++) valids[i] = 0;

    ifstream input("input.txt");

    string s;

    do{
        getline(input, s);
        if(!s.length()) break;
        bool* binary = convertHexToBinary(s);

        cacheAccesses++;

        int request = binary[0];

        if(request == 0) readAccesses++;
        if(request == 1) writeAccesses++;

        int tagS    = convertBinaryToInt(binary, 1,                 tagBits-1);
        int indexS  = convertBinaryToInt(binary, tagBits,           tagBits+indexBits-1);
        int offS    = convertBinaryToInt(binary, tagBits+indexBits, 31);
        int blockAdd= convertBinaryToInt(binary, 1,                 tagBits+indexBits-1);

        int comp=0;
        int cap=0;
        int conf=0;

        if(binarySearch(data, dataSize, blockAdd)){
            comp++;
            compMisses++;
            if(request == 0) readMisses++;
            if(request == 1) writeMisses++;
            data = insertElement(data, dataSize, blockAdd);
            dataSize++;
        }

        bool tagEx = searchS0(Cache[indexS], tagS);

        if(replace == 0){
            if(tagEx == 0){
                int random = rand()%valids[indexS];
                insertCacheBlock(random, Cache[indexS], tagS);
                valids[indexS]++;

                struct cacheBlock* temp = Cache[indexS];
                int i=0;
                while(temp->next!=NULL && i<setSize){
                    temp=temp->next;
                    i++;
                }

                if(temp->next!=NULL){
                    temp->next = NULL;
                    valids[indexS]--;

                    if(assoc==0) capMisses++;
                    else confMisses++;


                }
            }
        }

        if(replace == 1){
            if(tagEx == 1) LRU1(Cache[indexS], tagS, request);
            else{
                insertCacheBlock(0, Cache[indexS], tagS);

                valids[indexS]++;

                struct cacheBlock* temp = Cache[indexS];
                int i=0;
                while(temp->next!=NULL && i<setSize){
                    temp=temp->next;
                    i++;
                }

                if(temp->next!=NULL){
                    temp->next = NULL;
                    valids[indexS]--;

                    if(assoc==0) capMisses++;
                    else confMisses++;

                }
            }

            if(replace == 2){
                int i=0;
                while(i<setSize-1){
                    int goRight=trees[indexS][i];
                    trees[indexS][i] = (trees[indexS][i]+1)%2;
                    i = 2*i + goRight;
                }
                
                i -= setSize;
                struct cacheBlock* temp = Cache[indexS]->next;

                while(i){
                    temp=temp->next;
                    i--;
                }

                if(temp->validBit == 0){
                    temp->tag =  tagS;
                    temp->validBit = 1;
                    temp->dirtyBit = 0;
                    valids[indexS]++;
                }

                else{
                    if(valids[indexS] == setSize){
                        if(assoc == 0) capMisses++
                        else confMisses++;
                    }

                    temp->tag = tagS;
                    if(request == 1){
                        if(temp->dirtyBit == 1) dirtyBlocksEvic++;
                        else temp->dirtyBit = 1;
                    }
                }
            }
        }


    }while(s.length());



    input.close(); 


    

    cout << cacheSize << "\t#Cache size\n";
    cout << blockSize << "\t#Cache line size\n";

    if(assoc == 0)      cout << "Fully-associative cache\n";
    else if(assoc == 1) cout << "Direct-mapped cache\n";
    else                cout << "Set-associative cache\n";

    if(replace == 0)        cout << "Random Replacement\n";
    else if(replace == 1)   cout << "LRU Replacement\n";
    else if(replace == 2)   cout << "Pseudo-LRU Replacement\n";
    

    cout << cacheAccesses   << "\t#Cache accesses\n";
    cout << readAccesses    << "\t#Read accesses\n";
    cout << writeAccesses   << "\t#Write accesses\n";

    cout << cacheMisses     << "\t#Cache misses\n";
    cout << compMisses      << "\t#Compulsory misses\n";
    cout << capMisses       << "\t#Capacity misses\n";
    cout << confMisses      << "\t#Conflict misses\n";

    cout << readMisses      << "\t#Read misses\n";
    cout << writeMisses     << "\t#Write misses\n";

    cout << dirtyBlocksEvic << "\t#Dirty blocks evicted\n";


    /*
    ofstream output("output.txt");

    output << cacheSize << "\t#Cache size\n";
    output << blockSize << "\t#Cache line size\n";

    if(assoc == 0)      output << "Fully-associative cache\n";
    else if(assoc == 1) output << "Direct-mapped cache\n";
    else                output << "Set-associative cache\n";

    if(replace == 0)        output << "Random Replacement\n";
    else if(replace == 1)   output << "LRU Replacement\n";
    else if(replace == 2)   output << "Pseudo-LRU Replacement\n";
    

    output << cacheAccesses     << "\t#Cache accesses\n";
    output << readAccesses      << "\t#Read accesses\n";
    output << writeAccesses     << "\t#Write accesses\n";

    output << cacheMisses       << "\t#Cache misses\n";
    output << compMisses        << "\t#Compulsory misses\n";
    output << capMisses         << "\t#Capacity misses\n";
    output << confMisses        << "\t#Conflict misses\n";

    output << readMisses        << "\t#Read misses\n";
    output << writeMisses       << "\t#Write misses\n";

    output << dirtyBlocksEvic   << "\t#Dirty blocks evicted\n";
  
    output.close();
    */
}