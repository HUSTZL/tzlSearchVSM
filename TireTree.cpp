#ifndef TIRETREE
#define TIRETREE
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
#define MAX 26
typedef struct TrieNode {                   //Trie结点声明  
    bool isWord;                            //标记该结点处是否构成单词  
    int id;
    struct TrieNode *next[MAX];             //儿子分支  
} Trie;
class TrieTree {
    private:
        int totWordnum = 0;
        TrieNode *root;
        ofstream fout;
    public: TrieTree() {
            root = (TrieNode*) malloc (sizeof(TrieNode));
            for(int i = 0; i < MAX ; i++)
                root->next[i] = NULL;
            root->id = root->isWord = 0;
        }
    public: int gettotWordnum() { //获取总的不同的单词的数目
        return totWordnum;
    }
    public: void settotWordnum(int num) { //在非首次创建时设立单词的数目
        totWordnum = num;
    }
    public: void insert(string s) { //在首次创建时在字典树中插入单词
        TrieNode *p = root;
        for(int i = 0; i < s.size(); i++) {
            if(p->next[s[i] - 'a'] == NULL) {
                TrieNode *q = (TrieNode*) malloc (sizeof(TrieNode));
                for(int j = 0; j < MAX; j++)
                    q->next[j] = NULL;
                q->isWord = false;
                q->id = 0;
                p->next[s[i] - 'a'] = q;
                p = p->next[s[i] - 'a'];
            }
            else 
                p = p->next[s[i] - 'a'];
        }
        p->isWord = true;
        if(!(p->id))
            p->id = ++ totWordnum;
    }
    public: void insert(string s,int id) { //在非首次创建时在字典树中插入单词
        TrieNode *p = root;
        for(int i = 0; i < s.size(); i++) {
            if(p->next[s[i] - 'a'] == NULL) {
                TrieNode *q = (TrieNode*) malloc (sizeof(TrieNode));
                for(int i = 0; i < MAX; i++)
                    q->next[i] = NULL;
                q->isWord = false;
                q->id = 0;
                p->next[s[i] - 'a'] = q;
                p = p->next[s[i] - 'a'];
            }
            else 
                p = p->next[s[i] - 'a'];
        }
        p->isWord = true;
        if(!(p->id))
            p->id = id;
    }
    public: int getId(string s) { //获取一个单词的Id，单词不存在或单词为空时返回0
        if(s.length() == 0)
            return 0;
        TrieNode* p = root;
        for (int i = 0; i < s.length(); i++)
            if(p->next[s[i] - 'a'] != NULL)
                p = p->next[s[i] - 'a'];
            else 
                return 0;
        if(p != NULL && p->isWord)
            return p->id;
        else 
            return 0;
    }
    private: void dfs(TrieNode *p, char* word, int len) {
        if(p->isWord) {
            fout.write(word, 30*sizeof(char));
            fout.write((char*)&(p->id), sizeof(int));
        }
        for(int i = 0; i < MAX; i++) {
            word[len++] = 'a' + i;
            word[len] = '\0';
            if(p->next[i])
                dfs(p->next[i], word, len);
            word[--len] = '\0';
        }
    }
    public: void printWords(string path) { //上述两个函数实现信息向二进制文件WordsIdList.data的输出。
        fout.open(path, ios::out | ios::binary);
        char word[30]; int len = 0;
        fout.write((char*)&totWordnum, sizeof(int));
        dfs(root, word, len);
    }
};
#endif