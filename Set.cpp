#ifndef SET
#define SET
#include <string>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "LinkedList.cpp"
#define Maxn 20000+10
class Set {
    private:
        int lc[Maxn],rc[Maxn],fa[Maxn],size[Maxn],num[Maxn];
        int data[Maxn];
        LinkedList posList[Maxn];
        int root = 0,tot = 0,pos = 0, differentNum = 0;
        ofstream out;
    public: Set () {
        for(int i = 0; i < Maxn; i++)
            lc[i] = rc[i] = fa[i] = size[i] = num[i] = 0;
    }
    private: void updata (int u) {
        size[u]=size[lc[u]]+size[rc[u]]+num[u];
        return ;
    }
    private: void zig (int x) { //右旋
        int y=fa[x],z=fa[y];
        fa[x]=z;fa[y]=x;fa[rc[x]]=y;
        if(lc[z]==y)		lc[z]=x;
        else				rc[z]=x;
        lc[y]=rc[x];rc[x]=y;
        updata(y);
    }
    private: void zag (int x) { //左旋
        int y=fa[x],z=fa[y];
        fa[x]=z;fa[y]=x;fa[lc[x]]=y;
        if(lc[z]==y)		lc[z]=x;
        else				rc[z]=x;
        rc[y]=lc[x];lc[x]=y;
        updata(y);
    }
    private: void splay (int x) { //将x节点绕至根节点
        while(fa[x]) {
            int y=fa[x],z=fa[y];
            if(z) {
                if(lc[z]==y)
                    if(lc[y]==x)		zig(x),zig(x);
                    else 				zag(x),zig(x);
                else 
                    if(lc[y]==x)		zig(x),zag(x);
                    else 				zag(x),zag(x);
            }
            else 	
                if(lc[y]==x)			zig(x);
                else 					zag(x);
        }
        updata(x);
        root=x;
    }
    private: void insert (int u,int key,int pos) {
        if(key < data[u]) {
            if(lc[u])
                insert(lc[u], key, pos);
            else {
                lc[u]=++tot;
                data[tot]=key;
                fa[tot]=u;
                size[tot]=1;
                num[tot]=1;
                posList[tot].init();
                posList[tot].push_back(pos);
                splay(tot);
            }            
        }
        else if(key == data[u]) {
            num[u]++;
            posList[u].push_back(pos);
            splay(u);
            return ;
        }
        else {
            if(rc[u]) 
                insert(rc[u], key, pos);
            else {
                rc[u]=++tot;
                data[tot]=key;
                fa[tot]=u;
                size[tot]=1;
                num[tot] = 1;
                posList[tot].init();
                posList[tot].push_back(pos);
                splay(tot);
            }            
        }
    }
    private: void MidOrderDfs(int root, int nowDoc) { //中序遍历，输出文档初始化结果
        if(!root)
            return ;
        MidOrderDfs(lc[root], nowDoc);
        out.write((char*)&data[root], sizeof(int));
        int totDocFrequency = 1;
        out.write((char*)&totDocFrequency, sizeof(int));
        out.write((char*)&num[root], sizeof(int));
        out.write((char*)&nowDoc, sizeof(int));
        out.write((char*)&num[root], sizeof(int));
        Node *p = posList[root].getBegin();
        while(p != NULL) {
            out.write((char*)&(p->num), sizeof(int));
            p = p->next;
        }
        //cout << endl;
        MidOrderDfs(rc[root], nowDoc);
    }
    public: void Insert (int key,int pos) { //外部插入调用接口 其调用insert函数
        if(!root) {
            root=++tot;data[root]=key;size[root]=num[root]=1;
            posList[root].init();
            posList[root].push_back(pos);
        }
        else 
            insert(root, key, pos);
    }
    public: void Search(string path,int nowDoc) { //外部输出调用接口
        out.open(path, ios::out | ios::binary);
        getDifferentNumber(root);
        out.write((char*)&differentNum, sizeof(int));
        MidOrderDfs(root, nowDoc);
    }
    public: void getDifferentNumber(int root) {
        if(!root)
            return ;
        getDifferentNumber(lc[root]);
        differentNum++;
        getDifferentNumber(rc[root]);
    }
};
#endif