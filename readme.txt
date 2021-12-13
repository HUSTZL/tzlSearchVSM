Indexer

    文档以int编码 保存至docIndex.data 
        totDocsNumber 平均词数
            文档 id 文章的总词数

    每个单词与整数建立映射关系 存储在WordsIdList.data 和 WordPostionList.data//读进内存
        totWordsNumber
            单词 id 
        totWordsNumber
            在倒排索引中的位置信息
    （在内存中 单词与id采用字典树存储 而位置信息单独列表存储）
    （编号从1开始 0代表空单词)

    维护 dictionary.data(即倒排索引) 每个word按id构建posts //外存读写
        totTermNum
            id totDoc totFrequency 
                docID docFrequency 所有的位置信息（链表表示）
    （0号 空单词 0 0 0）
    class TrieTree {

    }
    class InvertedIndex{
        how to build?
        第一步：收集所有的(termID, docID, freq)的元组
        第二步：本地对收集到的元组按照termID进行排序，然后把结果写回到本地磁盘文件文件 (make “runs”)
            totTermNum
                id totDoc totFrequency  
                    docID docFrequency 所有的位置信息（链表表示）
        第三步：对第二步的结果进行归并排序(merge)，就可以创建一个简单的倒排索引
    }
    Build main
Scorer
    Vector Placement：Term Frequency Vector 
    Inverse Document Frequency(IDF)
        向量空间 yi = c(Wi, d) * IDF(Wi)
        c(Wi, d) count of word Wi
        IDF(Wi) = log[(M+1)/k]，其中M表示document的总数，k表示包含词W的document总数
    c(Wi, d)不合理，严格控制一个出现了很多次的词(term)对最终结果的贡献了，TF Transformation
        TF(Wi, d) = BM25(c(Wi, d)) BM25(x) = ((k+1)x) / (x+k)
    文本长度归一化(Document Length Normalization)
    
    进行评分，排序
Feedback main 
    布尔查询 Set交补
    单个查询    拉出InvertedIndex
    词组查询    信息检索导论P33
    
    利用Scorer
