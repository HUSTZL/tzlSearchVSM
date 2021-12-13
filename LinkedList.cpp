#ifndef _LINKEDLIST_
#define _LINKEDLIST_
#include <cstdio>
#include <cstdlib>
using namespace std;
typedef struct Node { //节点的定义
    int num;
    struct Node *next;
} Node;
class LinkedList {
    private:
        Node *start;
        Node *end;// 记录尾指针，便于在末尾O（1）插入元素
    public: LinkedList() {
        start = end = (Node *)malloc(sizeof(Node));
        start->next = NULL;
    }
    public: void init() {
        start = end = (Node *)malloc(sizeof(Node));
        start->next = NULL;
    }
    public: void Free() { //释放内存
        Node *p = start;Node *q = p->next;
        while(q != NULL) {
            free(p);
            p = q;
            q = q->next;
        }
        return ;
    }
    public: void push_back(int num) { //在链表最后插入值
        Node *p = (Node*)malloc(sizeof(Node));
        p->num = num; p->next = NULL;
        end->next = p; end = p;
        return ;
    }
    public: Node* getBegin() { //获得首元素指针
        return start->next;
    }
    public: Node* getNext(Node *p) {
        return p->next;
    }
    public: void intersextion(LinkedList a) { //两有序链表求交集
        Node *q = a.getBegin();
        Node *p = start->next;
        Node *newstart, *newend;
        newstart = newend = (Node *)malloc(sizeof(Node));
        newstart->next = NULL;
        while(p != NULL && q != NULL) {
            if((p->num) < (q->num)) {
                p = p->next;
            }
            else if((p->num) == (q->num)) {
                Node* r = (Node *)malloc(sizeof(Node));
                r->num = p->num; r->next = NULL;
                newend->next = r; newend = r;
                p = p->next; q = q->next;
            }
            else if((p->num) > (q->num)) {
                q = q->next;
            }
        }
        Free(); a.Free();
        start = newstart;
        end = newend;
        return ;
    }
    public: void merge(LinkedList a) { //两有序链表求并集
        Node *q = a.getBegin();
        Node *p = start->next;
        Node *newstart, *newend;
        newstart = newend = (Node *)malloc(sizeof(Node));
        newstart->next = NULL;
        while(p != NULL && q != NULL) {
            if((p->num) < (q->num)) {
                Node* r = (Node *)malloc(sizeof(Node));
                r->num = p->num; r->next = NULL;
                newend->next = r; newend = r;
                p = p->next;
            }
            else if((p->num) == (q->num)) {
                Node* r = (Node *)malloc(sizeof(Node));
                r->num = p->num; r->next = NULL;
                newend->next = r; newend = r;
                p = p->next; q = q->next;
            }
            else if((p->num) > (q->num)) {
                Node* r = (Node *)malloc(sizeof(Node));
                r->num = q->num; r->next = NULL;
                newend->next = r; newend = r;
                q = q->next;
            }
        }
        while(p != NULL) {
            Node* r = (Node *)malloc(sizeof(Node));
            r->num = p->num; r->next = NULL;
            newend->next = r; newend = r;
            p = p->next;
        }
        while(q != NULL) {
            Node* r = (Node *)malloc(sizeof(Node));
            r->num = q->num; r->next = NULL;
            newend->next = r; newend = r;
            q = q->next;
        }
        Free(); a.Free();
        start = newstart;
        end = newend;
        return ;
    }
};
#endif