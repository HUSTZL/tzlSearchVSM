#ifndef INVERTEDINDEX
#define INVERTEDINDEX
#include <string>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "TireTree.cpp"
#include "Set.cpp"
#include "Heap.cpp"
using namespace std;
struct Document { //文档信息的结构化定义
    char name[100];
    int WordNumber;
};
struct TERM { //term信息的定义
    int id, totDoc, totFrequency;
    vector<int> docID;
    vector<int> docFrequency;
    vector<LinkedList> posinfo;
};
void out(ofstream &fout, TERM &term) { //输出term于指定文件，指定位置
    fout.write((char*)&term.id, sizeof(int));
    fout.write((char*)&term.totDoc, sizeof(int));
    fout.write((char*)&term.totFrequency, sizeof(int));
    for(int i = 0; i < term.totDoc; i++) {
        fout.write((char*)&term.docID[i], sizeof(int));
        fout.write((char*)&term.docFrequency[i], sizeof(int));
        Node *p = term.posinfo[i].getBegin();
        while(p != NULL) {
            fout.write((char*)&(p->num), sizeof(int));
            p = p->next;
        }
    }
}
void in(ifstream &fin, TERM &term) { //输入term于指定文件，指定位置
    fin.read((char*)&term.id, sizeof(int));
    fin.read((char*)&term.totDoc, sizeof(int));
    fin.read((char*)&term.totFrequency, sizeof(int));
    for(int i = 0; i < term.totDoc; i++) {
        int docId, docFre, pos; LinkedList poslist;
        poslist.init();
        fin.read((char*)&docId, sizeof(int));
        term.docID.push_back(docId);
        fin.read((char*)&docFre, sizeof(int));
        term.docFrequency.push_back(docFre);
        for(int j = 0; j < docFre; j++) {
            fin.read((char*)&(pos), sizeof(int));
            poslist.push_back(pos);
        }
        term.posinfo.push_back(poslist);
    }
}
class InvertedIndex {
    private:
        string path = "newsDoc";
        string TEMPpath = "TEMP";
        int totDocsNumber = 0;  
        double aveBookWordsNumber = 0;
        vector<Document> Books;
        vector<int> WordPostionList;
        TrieTree Words;
    private: void initDocsandWords() { // 初始化文档信息和字典树
        DIR *dir;
        struct dirent *ptr;
        if ((dir = opendir(path.c_str())) == NULL) {
            perror("Open dir error...");
            return;
        }
        while ((ptr = readdir(dir)) != NULL) {
            if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
                continue;
            string aNewBook = ptr->d_name;
            ifstream newfin;
            newfin.open(path + "/" + aNewBook);
            stringstream buffer;
            buffer << newfin.rdbuf();
            string str(buffer.str());
            int thisDocWordNumber = 0;
            for(int i = 0; i < str.size(); i++)
                if(str[i] <= 'Z' && str[i] >= 'A') 
                    str[i] = str[i] - 'A' + 'a';
            for(int i = 0; i < str.size(); i++) {
                char aNewWord[100];int top = 0;
                if(str[i] >= 'a' && str[i] <= 'z') {
                    int j = i;
                    for(; j < str.size(); j++) 
                        if(str[j] >= 'a' && str[j] <= 'z')
                            aNewWord[top++] = str[j];
                        else 
                            break;
                    i = j;
                    aNewWord[top] = '\0';
                    top = 0;
                    thisDocWordNumber++;
                    string NewWord = aNewWord;
                    Words.insert(NewWord);
                }
            }
            Document *aBook;
            aBook = (Document*)malloc(sizeof(Document));
            strcpy(aBook->name, aNewBook.c_str());
            aBook->WordNumber = thisDocWordNumber;
            Books.push_back(*aBook);
            totDocsNumber++;
        }
        closedir(dir);
        ofstream fout("docIndex.data", ios::out | ios::binary);
        for(int i = 0; i < totDocsNumber; i++)
            aveBookWordsNumber += (double)Books[i].WordNumber/totDocsNumber;
        fout.write((char *)&totDocsNumber, sizeof(int));
        fout.write((char *)&aveBookWordsNumber, sizeof(double));
        for(int i = 0; i < totDocsNumber; i++) {
            fout.write((char *)&Books[i], sizeof(Document));
        }
        fout.close();
        return ;
    }
    private: void initEveryDoc() { //每个文档进行初始化
        for(int nowDoc = 0; nowDoc < totDocsNumber; nowDoc++) {
            int thisDocWordNumber = 0;
            Set tempList;
            ifstream newfin;
            newfin.open(path + "/" + Books[nowDoc].name);
            stringstream buffer;
            buffer << newfin.rdbuf();
            string str(buffer.str());
            //cout << str;
            for(int i = 0; i < str.size(); i++)
                if(str[i] <= 'Z' && str[i] >= 'A') 
                     str[i] = str[i] - 'A' + 'a';
            for(int i = 0; i < str.size(); i++) {
                string aNewWord;
                if(str[i] >= 'a' && str[i] <= 'z') {
                    int j = i;
                    for(; j < str.size(); j++) 
                        if(str[j] >= 'a' && str[j] <= 'z')
                            aNewWord.push_back(str[j]);
                        else 
                            break;
                    i = j;
                    int Id = Words.getId(aNewWord);
                    thisDocWordNumber++;
                    tempList.Insert(Id, thisDocWordNumber);
                }    
            }
            tempList.Search(TEMPpath + "/" + "0" + to_string(nowDoc) + ".temp",nowDoc);
        }          
    }
    private: void mergeDoc(int startDoc, int k, int deep) { // 合并从startDoc开始的k个深度为deep-1的临时表，
        ifstream fin[MaxK];
        for(int i = 0; i < k; i++)
            fin[i].open(TEMPpath + "/" + to_string(deep-1) + to_string(startDoc + i) + ".temp", ios::in | ios::binary);
        ofstream fout;
        fout.open(TEMPpath + "/" + to_string(deep) + to_string(startDoc/k) + ".tmp", ios::out | ios::binary);
        int totTERMnum[MaxK], nowTERMnum[MaxK]; TERM nowTERM[MaxK];
        Heap heap;
        for(int i = 0; i < k; i++) {
            fin[i].read((char*)&totTERMnum[i], sizeof(int));
            nowTERMnum[i] = 0;
            if(nowTERMnum[i] + 1 <= totTERMnum[i]) {
                fin[i].read((char*)&nowTERM[i].id, sizeof(int));
                fin[i].read((char*)&nowTERM[i].totDoc, sizeof(int));
                fin[i].read((char*)&nowTERM[i].totFrequency, sizeof(int));
                nowTERMnum[i] ++;
                heap.push((MergeNode){nowTERM[i].id, i});
            }
        }
        int PreMinTermId = 0;
        TERM aNewTerm;
        aNewTerm.id = 0;
        aNewTerm.totDoc = 0;
        aNewTerm.totFrequency = 0;
        aNewTerm.docID.clear();
        aNewTerm.docFrequency.clear();
        aNewTerm.posinfo.clear();
        int totDifferentWordNum = 0;
        while(heap.Size()) {
            MergeNode nowMinNode = heap.pop();
            if(PreMinTermId == nowMinNode.termId) {
                aNewTerm.totDoc += nowTERM[nowMinNode.thisDoc].totDoc;
                aNewTerm.totFrequency += nowTERM[nowMinNode.thisDoc].totFrequency;
            }
            else if(PreMinTermId == 0) {
                aNewTerm.id = nowMinNode.termId;
                aNewTerm.totDoc += nowTERM[nowMinNode.thisDoc].totDoc;
                aNewTerm.totFrequency += nowTERM[nowMinNode.thisDoc].totFrequency;
                PreMinTermId = nowMinNode.termId;
            }
            else {
                out(fout, aNewTerm);
                totDifferentWordNum++;
                aNewTerm.id = nowMinNode.termId;
                aNewTerm.totDoc = nowTERM[nowMinNode.thisDoc].totDoc;
                aNewTerm.totFrequency = nowTERM[nowMinNode.thisDoc].totFrequency;
                aNewTerm.docID.clear();
                aNewTerm.docFrequency.clear();
                aNewTerm.posinfo.clear();
            }
            for(int i = 0; i < nowTERM[nowMinNode.thisDoc].totDoc; i++) {
                int nowReadDocId, nowReadDocFre;
                fin[nowMinNode.thisDoc].read((char*)&nowReadDocId, sizeof(int));
                fin[nowMinNode.thisDoc].read((char*)&nowReadDocFre, sizeof(int));
                aNewTerm.docID.push_back(nowReadDocId);
                aNewTerm.docFrequency.push_back(nowReadDocFre);
                LinkedList nowReadDocPosList;
                nowReadDocPosList.init();
                for(int j = 0; j < nowReadDocFre; j++) {
                    int pos;
                    fin[nowMinNode.thisDoc].read((char*)&pos, sizeof(int));
                    nowReadDocPosList.push_back(pos);
                }
                aNewTerm.posinfo.push_back(nowReadDocPosList);
            }
            PreMinTermId = nowMinNode.termId;
            if(nowTERMnum[nowMinNode.thisDoc] + 1 <= totTERMnum[nowMinNode.thisDoc]) {
                nowTERMnum[nowMinNode.thisDoc] = nowTERMnum[nowMinNode.thisDoc] + 1;
                fin[nowMinNode.thisDoc].read((char*)&nowTERM[nowMinNode.thisDoc].id, sizeof(int));
                fin[nowMinNode.thisDoc].read((char*)&nowTERM[nowMinNode.thisDoc].totDoc, sizeof(int));
                fin[nowMinNode.thisDoc].read((char*)&nowTERM[nowMinNode.thisDoc].totFrequency, sizeof(int));
                heap.push((MergeNode){nowTERM[nowMinNode.thisDoc].id, nowMinNode.thisDoc});
            }
        }
        if(aNewTerm.id != 0) {
            out(fout, aNewTerm);          
            totDifferentWordNum++;
        }
        fout.close();//关键
        ifstream finCopy;
        finCopy.open(TEMPpath + "/" + to_string(deep) + to_string(startDoc/k) + ".tmp", ios::in | ios::binary);
        ofstream foutCopy;
        foutCopy.open(TEMPpath + "/" + to_string(deep) + to_string(startDoc/k) + ".temp", ios::out | ios::binary);
        foutCopy.write((char*)&totDifferentWordNum, sizeof(int));
        for(int i = 0; i < totDifferentWordNum; i++) {
            TERM term;
            in(finCopy, term);
            out(foutCopy, term);
        }
    }
    private: int Merge() { //合并文档，形成最终的倒排索引
        const int K = 200;
        int nowTotDocNumber = totDocsNumber, deep = 1;
        int nextDeepTotDocNumber = 0;
        while(nowTotDocNumber != 1) {
            for(int i = 0; i < nowTotDocNumber; i+=K) {
                mergeDoc(i, min(K, nowTotDocNumber - i), deep);
                nextDeepTotDocNumber++;
            }
            nowTotDocNumber = nextDeepTotDocNumber;
            nextDeepTotDocNumber = 0;
            deep = deep + 1;
        }
        return deep - 1;
    }
    private: void outInfomation(int finalDeep) { //输出内存中全部信息
        ofstream foutWordLish, foutDictionary;
        Words.printWords("WordsIdList.data");
        foutWordLish.open("WordPostionList.data", ios::out | ios::binary);
        foutDictionary.open("dictionary.data", ios::out | ios::binary);
        ifstream fin;
        fin.open(TEMPpath + "/" + to_string(finalDeep) + to_string(0) + ".temp", ios::in | ios::binary);
        int totTERMnum;
        fin.read((char*)&totTERMnum, sizeof(int));
        int newtotTERMnum = totTERMnum + 1;
        foutDictionary.write((char*)&(newtotTERMnum), sizeof(int));
        WordPostionList.push_back(foutDictionary.tellp());
        int zero = 0;
        foutDictionary.write((char*)&(zero), sizeof(int));
        foutDictionary.write((char*)&(zero), sizeof(int));
        foutDictionary.write((char*)&(zero), sizeof(int));
        for(int i = 1; i <= totTERMnum; i++) {
            TERM aNewTERM;
            in(fin, aNewTERM);
            WordPostionList.push_back(foutDictionary.tellp());
            out(foutDictionary, aNewTERM);
        }
        foutWordLish.write((char*)&newtotTERMnum, sizeof(int));
        for(int i = 0; i <= totTERMnum; i++) 
            foutWordLish.write((char*)&WordPostionList[i], sizeof(int));
    }
    void ReadWordsIdList() { // 读入内存中应有的信息
        ifstream fin1; 
        fin1.open("docIndex.data", std::ifstream::binary);
        fin1.read((char*)&totDocsNumber, sizeof(int));
        fin1.read((char*)&aveBookWordsNumber, sizeof(double));
        for(int i = 0; i < totDocsNumber; i++) {
            Document doc;
            fin1.read((char*)&doc, sizeof(Document));
            Books.push_back(doc);
        }
        fin1.close();
        ifstream fin2;
        fin2.open("WordsIdList.data", std::ifstream::binary);
        int totWordNum = 0;
        fin2.read((char*)&totWordNum, sizeof(int));
        Words.settotWordnum(totWordNum);
        for(int i = 1; i <= totWordNum; i++) {
            char word[30];int id;string Word;
            fin2.read((char*)word, 30*sizeof(char));
            fin2.read((char*)&id, sizeof(int));
            Word = word;
            Words.insert(Word, id);
        }
        int totTERMnum = 0;
        ifstream fin3;
        fin3.open("WordPostionList.data", ios::binary);
        fin3.read((char*)&totTERMnum, sizeof(int));
        for(int i = 0; i < totTERMnum; i++) {
            int dictionarypos = 0;
            fin3.read((char*)&dictionarypos, sizeof(int));
            WordPostionList.push_back(dictionarypos);
        }
    }
    public: void Build() {
        ifstream fin1;
        fin1.open("WordsIdList.data", std::ofstream::binary);
        if(fin1.is_open()) {
            ReadWordsIdList();
        }
        else {
            initDocsandWords();//为每一个新闻编号
                        //构建字典树
            initEveryDoc();
                        //收集所有的(termID, docID, freq)的元组
                        //本地对收集到的元组按照termID进行排序，
                        //然后把结果写回到本地磁盘文件文件
            int finalDeep = Merge();//对第二步的结果进行归并排序(merge)，就可以创建一个简单的倒排索引            
            outInfomation(finalDeep);
        }
    }
    public: int getWordId(string word) {
        return Words.getId(word);
    }
    public: int gettotDocsNumber() {
        return totDocsNumber;
    }
    public: int getReadPosion(int wordid) {
        return WordPostionList[wordid];
    }
    public: int getBookWordsNumber(int bookid) {
        return Books[bookid].WordNumber;
    }
    public: double getaveBookWordsNumber() {
        return aveBookWordsNumber;
    }
    public: char* getDocName(int docid) {
        return Books[docid].name;
    }
};
#endif