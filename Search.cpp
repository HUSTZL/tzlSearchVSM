#ifndef SEARCH
#define SEARCH
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#define MAXSEARCHWORDNUM 1000
#define MAXDOCXNUM 2000
#include "InvertedIndex.cpp"
using namespace std;
class VSMcross {//叉积结果
    public:
        int docId;
        double crossans;
    void operator = (const VSMcross &D) { 
        docId = D.docId;
        crossans = D.crossans;
    }
};
struct ExpressionNode {
    int n; // 0 运算符 1 词 n>1 长度为n的短语
    char operation;
    LinkedList keyword;
    LinkedList Set;
};
class Search {
    private: 
        InvertedIndex a;
        double k = 1.5;
        double b = 0.75;
        VSMcross tempstore[MAXDOCXNUM];
        int precede[6][6] = {
	        {0	},
	        {0, 1, -1, -1, 1, 1},
	        {0, 1,  1, -1, 1, 1},
	        {0,-1, -1, -1, 0, 0},
	        {0, 1,  1,  0, 1, 1},
	        {0,-1, -1, -1, 0, 0},
        } ;
    private: void Sort(VSMcross *a, int l, int r) {//将搜索结果按相关程度用归并排序排序
	    if(l >= r)
		    return ;
	    int mid = (l + r) >> 1;
	    Sort(a, l, mid);Sort(a, mid+1, r);
	    int lpos = l, rpos = mid+1, temp = l;
	    while(lpos <= mid && rpos <= r) {
		    if(a[lpos].crossans > a[rpos].crossans)
			    tempstore[temp++] = a[lpos++];
		    else
			    tempstore[temp++] = a[rpos++];
	    }
	    while(lpos <= mid) 
		    tempstore[temp++] = a[lpos++];
	    while(rpos <= r) 
		    tempstore[temp++] = a[rpos++];
	    for(int i = l; i <= r; i++)
		    a[i] = tempstore[i];		
	    return ;
    }
    private: void searchVSM(string str) {//利用向量空间模型搜索
        vector<int> keyword;
        int len = 0;
        while(str[len] != '\0')
            len++;
        for(int i = 0; i < len; i++)
            if(str[i] <= 'Z' && str[i] >= 'A') 
                str[i] = str[i] - 'A' + 'a';
        for(int i = 0; i < len; i++) {
            string aNewWord;
            if(str[i] >= 'a' && str[i] <= 'z') {
                int j = i;
                for(; j < len; j++) 
                if(str[j] >= 'a' && str[j] <= 'z')
                    aNewWord.push_back(str[j]);
                else 
                    break;
                i = j;
                if(a.getWordId(aNewWord) != 0)
                    keyword.push_back(a.getWordId(aNewWord));
            }
        }
        VSMcross result[MAXDOCXNUM];
        for(int i = 0; i < a.gettotDocsNumber(); i++) {
            result[i].docId = i;
            result[i].crossans = 0;
        }
        for(int i = 0; i < keyword.size(); i++) {
            ifstream fin;TERM term;
            fin.open("dictionary.data", ios::in | ios::binary);
            fin.seekg(a.getReadPosion(keyword[i]), ios::beg);
            in(fin, term);
            //cout << term.id << " " << term.totDoc << " " << term.totFrequency << endl;
            for(int j = 0; j < term.totDoc; j++) {
                result[term.docID[j]].crossans += 
                    (term.docFrequency[j]*(k+1)*log((double)(a.gettotDocsNumber()+1) /term.totDoc)) /
                    (term.docFrequency[j] + (k*(1-b+b*a.getBookWordsNumber(term.docID[j])/a.getaveBookWordsNumber())));
            }
        }
        Sort(result, 0, a.gettotDocsNumber()-1);
        keyword.clear();
        int totansnum = 0;
        for(int i = 0; i < min(a.gettotDocsNumber(), 20); i++) 
            if(result[i].crossans > 0) {
                cout << "\t" << a.getDocName(result[i].docId) << endl; 
                totansnum++;               
            }
            else    
                break;
        if(totansnum == 0)
            printf("\tFind No Solution!\n");
    }
    private: int getOptId(char a) {
	    if(a=='|')		return 1;
	    if(a=='&')		return 2;
	    if(a=='(')		return 3;
	    if(a==')')		return 4;
	    if(a=='#')		return 5;
        else            return 0;
    }
    private: int Precede(char a, char b) {
	    int idA = getOptId(a), idB = getOptId(b);
	    return precede[idA][idB];
    } 
    private: LinkedList getDocSet(ExpressionNode expresionnnode) { //单词查询和词组查询
        int n = expresionnnode.n;
        if(n == 1) { //单词查询
            LinkedList ans;
            ans.init();
            ifstream fin;TERM term;
            fin.open("dictionary.data", ios::in | ios::binary);
            fin.seekg(a.getReadPosion(expresionnnode.keyword.getBegin()->num), ios::beg);
            in(fin, term);
            for(int i = 0; i < term.totDoc; i++)
                ans.push_back(term.docID[i]);
            return ans;
        }
        else { // 词组查询
            LinkedList DocSet[MAXSEARCHWORDNUM], ans;
            ans.init();
            Node *p = expresionnnode.keyword.getBegin();
            for(int i = 0; i < n; i++) {
                DocSet[i].init();
                ifstream fin;TERM term;
                fin.open("dictionary.data", ios::in | ios::binary);
                fin.seekg(a.getReadPosion(p->num), ios::beg);
                in(fin, term);
                for(int j = 0; j < term.totDoc; j++)
                    DocSet[i].push_back(term.docID[j]);
                fin.close();
                p = p->next;
            }
            for(int i = 1; i < n; i++)
                DocSet[0].intersextion(DocSet[i]);
            ifstream fin;
            TERM term[MAXSEARCHWORDNUM];
            p = expresionnnode.keyword.getBegin();
            for(int i = 0; i < n; i++) {
                fin.open("dictionary.data", ios::binary);
                fin.seekg(a.getReadPosion(p->num), ios::beg);
                in(fin, term[i]);
                fin.close();
                p = p->next;
            }
            p = DocSet[0].getBegin();
            LinkedList poslist[MAXSEARCHWORDNUM];
            Node *posNode[MAXSEARCHWORDNUM];
            for(int i = 0; i < n; i++) 
                poslist[i].init();
            while(p != NULL) {
                int nowDocId = p->num;
                for(int i = 0; i < n; i++) {
                    poslist[i].Free();
                    poslist[i].init();
                    int L = 0, R = term[i].totDoc-1, mid, ans;
                    while(L <= R) {
                        mid = (L + R) >> 1;
                        if(term[i].docID[mid] == nowDocId) {
                            ans = mid;
                            break;
                        }
                        else if(term[i].docID[mid] < nowDocId)
                            L = mid + 1;
                        else 
                            R = mid - 1;
                    }
                    Node *q = term[i].posinfo[ans].getBegin();
                    while(q != NULL) {
                        poslist[i].push_back(q->num);
                        q = q->next;
                    }
                }
                for(int i = 0; i < n; i++) 
                    posNode[i] = poslist[i].getBegin();
                int flag = 1;
                while(posNode[0] != NULL) {
                    for(int i = 1; i < n; i++) {
                        while(posNode[i] && posNode[i-1] && (posNode[i]->num < posNode[i-1]->num + 1))
                            posNode[i] = posNode[i]->next;
                    }
                    flag = 1;
                    for(int i = 1; i < n; i++) 
                        if(posNode[i] == NULL || (posNode[i]->num != posNode[i-1]->num + 1)) {
                            flag = 0;
                            break;
                        }
                    if(flag) {
                        ans.push_back(nowDocId);
                        break;
                    }
                    else 
                        posNode[0] = posNode[0]->next;
                } 
                p = p->next;
            }
            return ans;
        }
    }
    private: void searchBool(string str) { //布尔查询
        ExpressionNode expression[MAXSEARCHWORDNUM];
        int totExpressionNode = 0;
        int len = 0;
        while(str[len] != '\0')
            len++;
        str[len++] = ' ';
        str[len++] = '\0';
        for(int i = 0; i < len; i++)
            if(str[i] <= 'Z' && str[i] >= 'A') 
                str[i] = str[i] - 'A' + 'a';
        for(int i = 0; i < len; i++) {
            string aNewWord;
            if(str[i] == '&' || str[i] == '|' || str[i] == '(' || str[i] == ')') {
                totExpressionNode++;
                expression[totExpressionNode].n = 0;
                expression[totExpressionNode].operation = str[i];
            }
            if(str[i] >= 'a' && str[i] <= 'z') {
                totExpressionNode++;
                expression[totExpressionNode].n = 0;
                expression[totExpressionNode].keyword.init();
                int j = i;int flag = 1;//指示现在是否是由单词到
                for(; j < len; j++) {
                    if(str[j] >= 'a' && str[j] <= 'z') {
                        aNewWord.push_back(str[j]);   
                        flag = 1;                     
                    }
                    else if(flag) {
                        expression[totExpressionNode].n++;
                        expression[totExpressionNode].keyword.push_back(a.getWordId(aNewWord));
                        aNewWord.clear();
                        flag = 0;
                    }
                    if(str[j] == '&' || str[j] == '|' || str[j] == '(' || str[j] == ')') {
                        j = j-1;
                        break;
                    }
                }
                i = j;
            }
        }
        /*for(int i = 1; i <= totExpressionNode; i++) {//(alibaba & work culture) | (COVID19 and its vaccine)
            if(expression[i].n == 0) {
                cout << "opt" << " " << expression[i].operation << endl;
            }
            else {
                cout << expression[i].n << " ";
                Node *p = expression[i].keyword.getBegin();
                while(p != NULL) {
                    cout << p->num << " ";
                    p = p->next;                    
                }
                cout << endl;
            }    
        }*/
        for(int i = 1; i <= totExpressionNode; i++) 
            if(expression[i].n > 0)
                expression[i].Set = getDocSet(expression[i]);
        char OPTR[MAXSEARCHWORDNUM];int topOPTR = 0;OPTR[topOPTR++] = '#'; 
        LinkedList OPND[MAXSEARCHWORDNUM];int topOPND = 0;
        expression[totExpressionNode+1].n = 0;
        expression[totExpressionNode+1].operation = '#';
        for(int i = 2; i <= totExpressionNode; i++)
            if(expression[i-1].n == 0 && expression[i].n == 0 && 
                expression[i-1].operation != '(' && expression[i-1].operation != ')' &&
                expression[i].operation != '(' && expression[i].operation != ')') {
                    cout << "\t INPUT WORNG!!!" << endl;
                    return ;
                }
        for(int i = 1; i <= totExpressionNode+1; i++) {
            if(expression[i].n > 0) {
                OPND[topOPND++] = expression[i].Set;
            }
            else {
                switch (Precede(OPTR[topOPTR-1], expression[i].operation)) {
                    case -1:
					    OPTR[topOPTR++] = expression[i].operation;
					    break;
				    case 0:
					    topOPTR--;
					    break;
				    case 1:
					    char OP = OPTR[--topOPTR];
                        LinkedList b, a;
                        if(topOPND >= 1)    b = OPND[--topOPND];
                        else                b.init();
                        if(topOPND >= 1)    a = OPND[--topOPND];
                        else                a.init();
					    if(OP == '&') {
                            a.intersextion(b);
                            OPND[topOPND++] = a;
                        }
                        else if(OP == '|') {
                            a.merge(b);   
                            OPND[topOPND++] = a;                         
                        }
                        i --;
					    break;
                }
            }
        }
        if(topOPTR != 1 && topOPND != 1) 
            cout << "\t INPUT WORNG!!!" << endl;
        else {
            int totansnum = 0;
            Node *p = OPND[topOPND-1].getBegin();
            while(p != NULL) {
                cout << "\t" << a.getDocName(p->num) << endl; 
                totansnum++;   
                p = p->next;    
            }
            if(totansnum == 0)
                printf("\tFind No Solution!\n");
        }
    }
    public: void main() {
        a.Build();
        cout << "Hello!We Have Built A InvertedIndex And We Have Prepared for you!" << endl;
        cout << "To Use Boolean Search, Please Use \"&\" \"|\" \"()\"!" << endl; 
        cout << "input EXIT to exit this program!" << endl;
        cout << "Highly Recommend to Only Use English Characters and \"&\" \"|\" \"()\"! And Reduce Your Use of Particle!!!" << endl;
        cout << "Created by Hou haofei 2021.12.12 in Wuhan" << endl;
        while(1) {
            char str[MAXSEARCHWORDNUM];int flag = 1, stopflag = 0;
            gets(str);
            for(int i = 0; str[i] != '\0'; i++)
                if(str[i]=='&' || str[i]=='|' || str[i]=='(' || str[i]==')')
                    flag = 0;
            for(int i = 0; str[i+3] != '\0'; i++)
                if(str[i]=='E' && str[i+1]=='X' && str[i+2]=='I' && str[i+3]=='T')
                    stopflag = 1;
            if(stopflag)
                break;
            if(flag) 
                searchVSM(str);
            else 
                searchBool(str);
        }
    }
};
#endif