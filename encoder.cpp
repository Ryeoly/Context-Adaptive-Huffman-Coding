#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <bitset>
using namespace std;

#pragma warning(disable:4996)

//Huffman tree struct
typedef struct Huf_Tree {
    Huf_Tree* LeftNode;
    Huf_Tree* RightNode;
    Huf_Tree* ParentNode;
    vector<int> ascy;            // ASCII codes that a node has as a child
    double probability;            // weight of node
}Huf_Tree;

//Huffman tree constructor
Huf_Tree* make_Node(vector<int> as, double prob) {
    Huf_Tree* node;
    node = new Huf_Tree();

    node->LeftNode = NULL;
    node->RightNode = NULL;
    node->ParentNode = NULL;
    node->ascy = as;
    node->probability = prob;

    return node;
}

/* The conversion code suitable for the ASCII code is returned. (like huffman_table) */
string make_codeword(Huf_Tree* tree, int asci) {
    string binary_code = "";
    Huf_Tree* node = tree;                  //unconditionally root

    // ASCII code not in input.txt
    if (*find(node->ascy.begin(), node->ascy.end() - 1, asci) != asci) {
        return "-1";            //context table���� basic table�� �Ѿ�� �Ϸ���
    }

    // Traversing the tree and looking for transformation codes
    while (1) {
        if (node->LeftNode == NULL && node->RightNode == NULL) { // leaf node. (already find code)
            break;
        }

        else {
            // If the left node has the corresponding ASCII code
            node = node->LeftNode;
            if (*find(node->ascy.begin(), node->ascy.end() - 1, asci) == asci) {
                binary_code = binary_code + "0";
                continue;
            }

            // If the right node has the corresponding ASCII code
            node = (node->ParentNode)->RightNode;
            binary_code = binary_code + "1";
        }
    }
    return binary_code;
}


void create_inupt_codehbs(map<int, Huf_Tree*> context_tree_tables, int input_number) {      //stuffing bit ���� ��ȯ
   /* input �����ؼ� ó���ϴ� �κ�, input�� code�� ��ȯ�� �ִ� �κ� */
    string code_code = "";
    FILE* fp2;
    if (input_number == 0) {
        fp2 = fopen("training_input.txt", "r");
    }
    else if (input_number == 1) {
        fp2 = fopen("test_input1.txt", "r");
    }
    else if (input_number == 2) {
        fp2 = fopen("test_input2.txt", "r");
    }
    else if (input_number == 3) {
        fp2 = fopen("test_input3.txt", "r");
    }
    else {
        printf("error open txt_file\n"); return;
    }

    if (!fp2) {
        printf("file2 open fail \n");
        return;
    }

    char tmp2;
    int preceding_word = -1;
    int evidence = 0;

    while (1) {
        tmp2 = fgetc(fp2);
        if (tmp2 == EOF) { break; }
        int ascy2 = tmp2;
        if (ascy2 >= 0 && ascy2 <= 127) {

            //code_code = code_code + make_codeword(iter->second, ascy2);   // �ڵ���� �Է¹޴� ��� ��ȯ �� ����
            map<int, Huf_Tree*>::iterator iterate;
            iterate = context_tree_tables.begin();
            for (int i = 0; i < context_tree_tables.size(); i++) {
                if (iterate->first == preceding_word) {                        // preceding�� �°� ���� �� �ٶ��
                    string trans_binary = "";
                    trans_binary = trans_binary + make_codeword(iterate->second, ascy2);
                    evidence++;
                    if (trans_binary == "-1") {
                        code_code = code_code + "1";
                        /*code_code = code_code + make_codeword(iter->second, ascy2);*/
                        bitset<8> fix_code(ascy2);
                        code_code = code_code + fix_code.to_string();
                        break;
                    }
                    code_code = code_code + trans_binary;
                    break;
                }
                iterate++;
            }


            if (evidence == 0) {
                /*code_code = code_code + make_codeword(iter->second, ascy2);   */
                bitset<8> fix_code(ascy2);
                code_code = code_code + fix_code.to_string();
            }
            preceding_word = ascy2;
            evidence = 0;
        }
    }
    fclose(fp2);

    //code_code = code_code + make_codeword(iter->second, 128);         // add EOD code
    code_code = code_code + "10000000";         // add EOD code
    int add_bit;         // add stuffing bit
    if (code_code.length() % 8 == 0) {
        add_bit = 0;
    }
    else if (code_code.length() % 8 != 0) {
        add_bit = 8 - (code_code.length() % 8);
    }

    for (int i = 0; i < add_bit; i++) {
        code_code = code_code + "0";
    }


    FILE* code_FILE;
    if (input_number == 0) {
        code_FILE = fopen("training_input_code.hbs", "wb");
    }
    else if (input_number == 1) {
        code_FILE = fopen("test_input1_code.hbs", "wb");
    }
    else if (input_number == 2) {
        code_FILE = fopen("test_input2_code.hbs", "wb");
    }
    else if (input_number == 3) {
        code_FILE = fopen("test_input3_code.hbs", "wb");
    }
    else { printf("error open code_file\n"); return; }

    int byte_w = 0;                                             // table.hbs�� ������ �Ͱ� ���� �˰���
    for (int i = 0; i < code_code.length(); i++) {

        if (code_code[i] == '1') {
            byte_w = byte_w + (int)pow(2, 7 - (i % 8));
        }

        if ((i % 8) == 7) {
            fputc(byte_w, code_FILE);
            byte_w = 0;
        }

    }
    fclose(code_FILE);



    return;
}


/* Used to combine two nodes with the smallest weight (when make tree) */
Huf_Tree* combine_Node(Huf_Tree* first_Tree, Huf_Tree* second_Tree) {
    Huf_Tree* node;
    node = new Huf_Tree();

    //among the two nodes, the node with the smaller weight is the left child.
    if (first_Tree->probability > second_Tree->probability) {
        node->LeftNode = second_Tree;
        node->RightNode = first_Tree;
    }
    else {
        node->LeftNode = first_Tree;
        node->RightNode = second_Tree;
    }
    node->ParentNode = NULL;

    // Combining vectors of two nodes
    vector<int> tmp = first_Tree->ascy;
    tmp.insert(tmp.end(), second_Tree->ascy.begin(), second_Tree->ascy.end());
    node->ascy = tmp;
    node->probability = first_Tree->probability + second_Tree->probability;

    // In order for the parent node to have the ASCII code of its children
    first_Tree->ParentNode = node;
    second_Tree->ParentNode = node;

    return node;
}


//context ���ڵ����� �� column�� �´� tree�� ����� ���ؼ� ������ �Լ�
map<int, Huf_Tree*> each_col_tree(double context_table[][128]) {
    map<int, Huf_Tree*> return_map;


    for (int a = 0; a < 128; a++) {      // 128�� �� ���鼭 �� �÷����� tree�� ����� ����
        multimap<double, vector<int>> value;
        if (context_table[128][a] != 0) {   // �ش� �÷��� tree�� ���� ��ġ�� �ִ��� Ȯ��

            for (int b = 0; b < 128; b++) {   // ���� row�� ���鼭 map�� Ȯ��, �ƽ�Ű �ڵ� �����ϱ�
                if (context_table[b][a] != 0) {
                    value.insert(pair<double, vector<int>>(context_table[b][a], { b }));
                }
            }
            value.insert(pair<double, vector<int>>(0, { 128 })); // Insert 128 ASCII code for EOD
            value.insert(pair<double, vector<int>>(1.1, { 130 }));      //context�� ���� �־����ϴ�. �ֳ��ϸ� encode�� binary �ڵ� ���ڵ� �߿� basic table�� �̵��϶��� ��ɾ �ʿ��ؼ� 

            map<double, vector<int>>::iterator it;
            double a_prob = 0;
            vector<int> a_vector;
            double b_prob = 0;
            vector<int> b_vector;
            int value_size = value.size();
            map<vector<int>, Huf_Tree*> vector_table;         // to make ASCII codes into one tree


            for (int i = 1; i < value_size; i++) {
                Huf_Tree* Node_a = NULL;
                Huf_Tree* Node_b = NULL;

                it = value.begin();                        // The one with the least probability = a
                a_prob = it->first;
                a_vector = it->second;
                if (a_vector.size() == 0) {
                    printf("error 1\n");
                    exit;
                }
                else if (a_vector.size() == 1) {            // only one ASCII code, there is no node
                    Node_a = make_Node(a_vector, a_prob);
                }
                else {
                    if (vector_table.size() != 0) {
                        vector<int> idx = a_vector;
                        Node_a = vector_table[idx];            // Take a tree out of a small tree table
                    }
                }
                value.erase(it++);                        // ù��° �����ϸ鼭 �ι�°�� Ȯ���� ���� �� ����



                b_prob = it->first;                        // The second least probable one = b
                b_vector = it->second;
                if (b_vector.size() == 0) {
                    printf("error 2\n");
                    exit;
                }
                else if (b_vector.size() == 1) {
                    Node_b = make_Node(b_vector, b_prob);
                }
                else {
                    if (vector_table.size() != 0) {
                        vector<int> idx2 = b_vector;
                        Node_b = vector_table[idx2];
                    }
                }
                value.erase(it);


                vector_table.erase(a_vector);
                vector_table.erase(b_vector);
                a_vector.insert(a_vector.end(), b_vector.begin(), b_vector.end());
                float tmp1 = a_prob + b_prob;
                // Combines two nodes into a small tree
                vector_table.insert(pair<vector<int>, Huf_Tree*>(a_vector, combine_Node(Node_a, Node_b)));
                //store small tree
                value.insert(pair<double, vector<int>>(tmp1, a_vector));
            }
            //������� ���� vector table���� �������� �ϼ��� tree �ϳ��� ���� �ְ�,
            //value���� ��� ������ ������ Ȯ���� 1�� �͸� ���´�. 

            map<vector<int>, Huf_Tree*>::iterator iter;
            iter = vector_table.begin();

            return_map.insert(pair<int, Huf_Tree*>(a, iter->second));
        }
    }

    return return_map;
}


int main(void)
{
    /* ���� basic table ����*/
    string fix_table_code = "";
    for (int i = 0; i < 129; i++) {
        bitset<8> int_to_binary(i);
        fix_table_code = fix_table_code + int_to_binary.to_string();
    }
    /* ���� �Ϸ� */

    double asci[128] = { 0, };
    double count = 0;
    multimap<double, vector<int>> value;         // probability, Ascii code
    double context_table[130][128] = { 0 };         // 2���� �迭�� preceding ���� �ƽ�Ű �ڵ� count [129���� count. Ȯ���� ���ϱ� ����], 
    // [130][]�� reduce table�� ���ֱ� ���ؼ�  


    /* input.txt file open , error check */
    fstream fp("training_input.txt");
    if (!fp) {
        printf("file open fail \n");
        return 0;
    }

    /* input_data.txt words count and check */
    char tmp;
    int preceding = -1;
    while (fp.get(tmp)) {

        if (count == 0) {
            count++;
            int ascy = tmp;
            preceding = ascy;               // ó������ preceding�� �����Ƿ� ����
            if (ascy >= 0 && ascy <= 127) {
                asci[ascy]++;                     // count Ascii code
            }
        }

        else {
            count++;
            int ascy = tmp;
            context_table[ascy][preceding]++;      // context�� �°� ������ �� count
            context_table[128][preceding]++;      // ���߿� Ȯ�� ���Ϸ��� �� �ƽ�Ű�� ���� Ƚ���� ���
            if (context_table[ascy][preceding] == 0) {
                context_table[129][preceding]++;   // reduce table�� �������ֱ� ���� 
            }
            if (ascy >= 0 && ascy <= 127) {
                asci[ascy]++;                     // count Ascii code
            }

            preceding = ascy;               // preceding �� �ٲٱ�
        }

    }
    fp.close();




    /* convert frequency to probability */
    for (int i = 0; i < 128; i++) {
        if (asci[i] != 0.0) {
            double percent = asci[i] / count;      // counting number / total number = probability
            value.insert(pair<double, vector<int>>(percent, { i }));
        }
        else if (asci[i] == 0) {
            value.insert(pair<double, vector<int>>(0.000000001, { i }));
        }

        // ���� �׳� ������ �ڵ�, �Ʒ��� context ������ �ڵ�

        if (context_table[128][i] != 0) {
            for (int j = 0; j < 128; j++) {
                context_table[j][i] = context_table[j][i] / context_table[128][i];   // column �ϳ� �����ؼ� �� �������鼭 Ȯ�� ���ϴ� ��
            }
        }

    }
    value.insert(pair<double, vector<int>>(0, { 128 })); // Insert 128 ASCII code for EOD
    // ������� ���� context_table�� Ȯ�� ���� ä�� ����� �Ǿ��ִ�. �̸� �������� basic ������ ��ȯ �ڵ� ���ϵ��� tree����� �� �ؾߵ�.

    /*���� context ������ �ڵ� ������ ���� �κ�.*/
    map<int, Huf_Tree*> context_tree_tables = each_col_tree(context_table);
    /* tables�� �� column �� tree�� �����Ǿ� �ִ�. */



    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* ���� ���� context tree map���� ���鼭 ������ ���̺� �ڵ� string���� �� �̾� �ٿ��� ������ �κ� */

    map<int, Huf_Tree*>::iterator iterat;
    iterat = context_tree_tables.begin();
    string context_table_code = "";
    bitset<8> number_of_tables(context_tree_tables.size());
    context_table_code = context_table_code + number_of_tables.to_string();      // the number of tables ���

    for (int i = 0; i < context_tree_tables.size(); i++) {                  // ���� context table code ��� 
        bitset<8> preceding_code(iterat->first);
        context_table_code = context_table_code + preceding_code.to_string();

        for (int j = 0; j <= 128; j++) {
            string str_tmp = "";

            if (j == 128) {
                str_tmp = make_codeword(iterat->second, 130);

                if (str_tmp.length() != 0) {
                    bitset<8> tmptmp3(130);
                    context_table_code = context_table_code + tmptmp3.to_string();

                    bitset<8> tmptmp_len(str_tmp.length());
                    context_table_code = context_table_code + tmptmp_len.to_string();

                    context_table_code = context_table_code + str_tmp;
                    str_tmp = "";
                }
            }

            str_tmp = make_codeword(iterat->second, j);

            if (str_tmp != "-1") {
                bitset<8> tmptmp(j);
                context_table_code = context_table_code + tmptmp.to_string();

                bitset<8> tmptmp_len(str_tmp.length());
                context_table_code = context_table_code + tmptmp_len.to_string();

                context_table_code = context_table_code + str_tmp;
            }
        }
        iterat++;
    }

    int add_stuffing = 8 - (context_table_code.length() % 8);
    for (int i = 0; i < add_stuffing; i++) {
        context_table_code = context_table_code + "0";
    }

    /* Write context_adaptive_huffman_table.hbs */
    FILE* table_FILE2 = fopen("context_adaptive_huffman_table.hbs", "wb");

    int byte_w2 = 0;                                       // converted to byte and saved
    for (int i = 0; i < context_table_code.length(); i++) {

        if (context_table_code[i] == '1') {
            byte_w2 = byte_w2 + (int)pow(2, 7 - (i % 8));            // 8bit�� ���� ������ش�.
        }

        if ((i % 8) == 7) {                                 // 8bit�� ������ ���� �� ���� �ʱ�ȭ 
            fputc(byte_w2, table_FILE2);
            byte_w2 = 0;
        }

    }
    fclose(table_FILE2);
    /* context table code ���� �Ϸ� */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* Write Huffman_table.hbs */
    FILE* table_FILE = fopen("huffman_table.hbs", "wb");

    int byte_w = 0;                                       // converted to byte and saved
    for (int i = 0; i < fix_table_code.length(); i++) {

        if (fix_table_code[i] == '1') {
            byte_w = byte_w + (int)pow(2, 7 - (i % 8));            // 8bit�� ���� ������ش�.
        }

        if ((i % 8) == 7) {                                 // 8bit�� ������ ���� �� ���� �ʱ�ȭ 
            fputc(byte_w, table_FILE);
            byte_w = 0;
        }

    }
    fclose(table_FILE);

    for (int i = 0; i < 4; i++) {
        create_inupt_codehbs(context_tree_tables, i);
    }



    return 0;
}