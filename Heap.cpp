#ifndef HEAP
#define HEAP
#define MaxK 230
using namespace std;
struct MergeNode { //堆维护外部排序要合并的节点
    int termId;
    int thisDoc;
};
bool operator < (MergeNode a, MergeNode b) { //以termId为第一关键字，thisDoc为第二关键字
    if(a.termId < b.termId)
        return 1;
    else if(a.termId == b.termId) {
        if(a.thisDoc < b.thisDoc)
            return 1;
        else 
            return 0;
    }
    else 
        return 0;
}
bool operator > (MergeNode a, MergeNode b) {
    return !(a<b);
}
class Heap {
    private:
        MergeNode a[MaxK];
        int size;
    public: Heap() {
        size = 0;
    }
    public: int Size() {
        return size;
    }
    public: void push(MergeNode key) { //压入节点
        size++; 
	    int i; 
	    for (i = size; i > 1 && a[i>>1] > key; i >>= 1) 
		    a[i]=a[i>>1]; 
	    a[i]=key;
    }
    public: MergeNode pop() { //弹出节点
        MergeNode p, ans = a[1];
        swap(a[1], a[size]);
        p = a[1];
        int s = 1; size = size - 1;
	    for(int j = 2*s; j <= size; s = j, j = j*2) {
		    if(j+1 <= size && a[j+1] < a[j])
			    j++;
		    if(p < a[j])
			    break;
		    a[s] = a[j];
	    }
	    a[s] = p;
        return ans;
    }
};
#endif