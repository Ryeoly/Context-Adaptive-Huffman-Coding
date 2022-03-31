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
    int ascy;            // Ascii code
    string code;         // covert code
}Huf_Tree;

//Huffman tree constructor
Huf_Tree* make_Node(int ascy, string code) {
    Huf_Tree* node;
    node = new Huf_Tree();

    node->LeftNode = NULL;
    node->RightNode = NULL;
    node->ParentNode = NULL;
    node->ascy = ascy;
    node->code = code;

    return node;
}


Huf_Tree* make_tree(map<int, string> table) {
    Huf_Tree* root = new Huf_Tree();

    map<int, string>::iterator it;
    it = table.begin();

    for (int i = 0; i < table.size(); i++) {
        Huf_Tree* current_node = root;

        for (int j = 0; j < it->second.length(); j++) {      //for���� �ٵ��� �Ѱ��� �ƽ�Ű�ڵ尪�� tree�� node�� ������ ��

           // ��ȯ�ڵ尡 0�̰� �̵� �����ҋ�
            if (it->second[j] == '0' && current_node->LeftNode != NULL) {
                current_node = current_node->LeftNode;
            }

            // ��ȯ �ڵ尡 0�̰� �̵� �Ұ��� ��
            else if (it->second[j] == '0' && current_node->LeftNode == NULL) {
                Huf_Tree* new_node = new Huf_Tree();      //�̵��� ��带 ����� �̵�
                new_node->ParentNode = current_node;
                current_node->LeftNode = new_node;
                current_node = new_node;
            }

            // ��ȯ �ڵ尡 1�̰� �̵� ������ �� 
            else if (it->second[j] == '1' && current_node->RightNode != NULL) {
                current_node = current_node->RightNode;
            }

            //��ȯ �ڵ尡 1�̰� �̵� �Ұ��Ҷ�
            else if (it->second[j] == '1' && current_node->RightNode == NULL) {
                Huf_Tree* new_node = new Huf_Tree();   //�̵��� ��带 ����� �̵�
                new_node->ParentNode = current_node;
                current_node->RightNode = new_node;
                current_node = new_node;
            }

            else {
                printf("error");
                return 0;
            }

            //��ȯ �ڵ� ������ �������� �̵��� �Ϸ�Ǿ�, �ش� ��忡 �ڽ��� �ƽ�Ű �ڵ�, ��ȯ �ڵ带 ����
            if (j == it->second.length() - 1) {
                current_node->ascy = it->first;
                current_node->code = it->second;
            }
        }

        it++;      //���� �ƽ�Ű �� Ʈ���� ������ ��
    }

    return root;
}

map<int, Huf_Tree*> make_context_trees(string str) {
    map<int, Huf_Tree*> return_trees;

    string number_of_table_str = str.substr(0, 8);
    str.erase(0, 8);
    int number_of_table = stoi(number_of_table_str, nullptr, 2);

    int count = 0;
    for (int i = 0; i < number_of_table; i++) {
        string preceding_binary = str.substr(0, 8);
        str.erase(0, 8);


        map<int, string> table;
        string first = "";                        // 8 bit , Ascii code
        string second = "";                        // 8 bit , convert code length
        string third = "";                        // convert code
        while (1) {

            if ((count % 3) == 0) {
                first = str.substr(0, 8);
                str.erase(0, 8);

                if (stoi(first, nullptr, 2) == 128) {   //eod ������ ���� preceding���� �Ѿ���� �׷��� break
                    string b = "";
                    string c = "";
                    b = str.substr(0, 8);
                    str.erase(0, 8);

                    c = str.substr(0, stoi(b, nullptr, 2));
                    str.erase(0, stoi(b, nullptr, 2));
                    table.insert(pair<int, string>(stoi(first, nullptr, 2), c));
                    break;
                }

            }

            else if ((count % 3) == 1) {
                second = str.substr(0, 8);
                str.erase(0, 8);
            }

            else {
                third = str.substr(0, stoi(second, nullptr, 2));   //use stoi, get second decimal value
                str.erase(0, stoi(second, nullptr, 2));
                table.insert(pair<int, string>(stoi(first, nullptr, 2), third));   //store table (�ƽ�Ű �ڵ庰 code)
                first = "";
                second = "";
                third = "";
            }
            count++;
        }
        int column = stoi(preceding_binary, nullptr, 2);
        return_trees.insert(pair<int, Huf_Tree*>(column, make_tree(table)));

    }
    return return_trees;
}



void decoder_code_hbs(int input_number, Huf_Tree* root, map<int, Huf_Tree*> context_trees) {
    /* code.hbs file ��� binary�� ���ڸ� string���� ���ٷ� ���� */
    string huf_codes = "";
    FILE* code_FILE;
    FILE* result_FILE;


    if (input_number == 0) {
        code_FILE = fopen("training_input_code.hbs", "rb");
        result_FILE = fopen("training_output.txt", "w+");
    }
    else if (input_number == 1) {
        code_FILE = fopen("test_input1_code.hbs", "rb");
        result_FILE = fopen("test_output1.txt", "w+");
    }
    else if (input_number == 2) {
        code_FILE = fopen("test_input2_code.hbs", "rb");
        result_FILE = fopen("test_output2.txt", "w+");
    }
    else if (input_number == 3) {
        code_FILE = fopen("test_input3_code.hbs", "rb");
        result_FILE = fopen("test_output3.txt", "w+");
    }
    else {
        printf("error open txt_file\n"); return;
    }



    while (code_FILE == NULL) {
        printf("failed open code file");
        return;
    }

    int d = fgetc(code_FILE);
    while (d != EOF) {
        bitset<8> tmp2(d);         //huf_code is binary number of huffman_code.hbs 
        huf_codes = huf_codes + tmp2.to_string();
        d = fgetc(code_FILE);
    }
    fclose(code_FILE);


    /* table�� �̿��� ���� tree�� �̿��� decoding start */
    /* ���� : root���� �ѱ��ھ� ��Ȳ�� �´� ��ġ�� �������� �̵��� node�� �ڽ��� ������ ã����*/
    int stuffing_ = 0;
    for (int i = huf_codes.length(); i > 0; i--) {
        if (huf_codes[i - 1] == '1') {
            stuffing_++;
            break;
        }
        stuffing_++;
    }

    int precede_char = -1;
    Huf_Tree* c_node = root;
    int evidence = 0;                                    // context table�� ������ �Ǵ��ϱ� ����
    int for_context_evidence = 0;                           //context table�� ���� c_node�� ������ � �ʿ�
    int to_basic_evidence = 0;                              //normal table���� c_node�� ������ ��ٴ� ����
    for (int i = 0; i < huf_codes.length() - stuffing_ + 1; i++) {
        map<int, Huf_Tree*>::iterator iterat;
        iterat = context_trees.begin();
        for (int j = 0; j < context_trees.size(); j++) {   // context�� ���� �����ϴ� for��
            if (iterat->first == precede_char && to_basic_evidence != 1) {
                vector<int>::iterator exist_context_table;
                evidence++;
                if (for_context_evidence == 0) {
                    c_node = iterat->second;
                    for_context_evidence++;
                }

                if (huf_codes[i] == '0') {      // ready move to the left
                    if (c_node->LeftNode == NULL) { // ������ �� �����Ƿ� �� ��ġ�� node�� ���� ���
                        if (c_node->ascy == 130) {  // context_table�� ���� �ܾ ���� ��
                            evidence = 0;
                            to_basic_evidence = 1;
                            for_context_evidence = 0;
                            c_node = root;
                            break;
                        }
                        fputc(c_node->ascy, result_FILE);
                        i--;
                        precede_char = c_node->ascy;
                        c_node = root;
                        for_context_evidence = 0;
                        break;
                    }
                    else {   //move
                        c_node = c_node->LeftNode;
                        break;
                    }
                }

                else if (huf_codes[i] == '1') {      // ready move to the right
                    if (c_node->RightNode == NULL) {   // ������ �� �����Ƿ� �� ��ġ�� node�� ���� ���
                        if (c_node->ascy == 130) {      // context_table�� ���� �ܾ ���� ��
                            evidence = 0;
                            to_basic_evidence = 1;
                            for_context_evidence = 0;
                            c_node = root;
                            break;
                        }
                        fputc(c_node->ascy, result_FILE);
                        i--;
                        precede_char = c_node->ascy;
                        c_node = root;
                        for_context_evidence = 0;
                        break;
                    }
                    else {
                        c_node = c_node->RightNode;
                        break;
                    }
                }

            }
            iterat++;
        }

        if (huf_codes[i] == '0' && evidence == 0) {      // ready move to the left
            if (c_node->LeftNode == NULL) { // ������ �� �����Ƿ� �� ��ġ�� node�� ���� ���
                if (c_node->ascy == 128) {  // EOD ã�� ��� ��!
                    break;
                }
                fputc(c_node->ascy, result_FILE);
                i--;
                precede_char = c_node->ascy;
                c_node = root;
                to_basic_evidence = 0;
            }
            else {   //move
                c_node = c_node->LeftNode;
            }
        }

        else if (huf_codes[i] == '1' && evidence == 0) {      // ready move to the right
            if (c_node->RightNode == NULL) {   // ������ �� �����Ƿ� �� ��ġ�� node�� ���� ���
                if (c_node->ascy == 128) {      // EOD ã�� ��� ��!
                    break;
                }
                fputc(c_node->ascy, result_FILE);
                i--;
                precede_char = c_node->ascy;
                c_node = root;
                to_basic_evidence = 0;
            }
            else {
                c_node = c_node->RightNode;
            }
        }
        evidence = 0;
    }
    fclose(result_FILE);
    return;
}


int main(void) {
    string EOD_ = "10000000";
    string stuffing_ = "";
    int count = 0;
    string get_binary = "";
    Huf_Tree* root = new Huf_Tree();

    /* table.hbs file ��� binary�� ���ڸ� string���� �� ���ٷ� ���� */
    FILE* table_FILE = fopen("huffman_table.hbs", "rb");
    while (table_FILE == NULL) {
        printf("failed open table file");
        return 0;
    }
    while (1) {
        int c = fgetc(table_FILE);
        if (c == EOF) {
            break;
        }

        bitset<8> tmp(c);
        get_binary = get_binary + tmp.to_string();   // get binary�� binary ���� ����
    }
    fclose(table_FILE);


    /* context_adapter_huffman_table.hbs file ��� binary�� ���ڸ� string���� �� ���ٷ� ���� */
    FILE* context_table_FILE = fopen("context_adaptive_huffman_table.hbs", "rb"); \
        string context_table_binary = "";
    while (context_table_FILE == NULL) {
        printf("failed open table context file");
        return 0;
    }
    while (1) {
        int c = fgetc(context_table_FILE);
        if (c == EOF) {
            break;
        }

        bitset<8> tmp_(c);
        context_table_binary = context_table_binary + tmp_.to_string();   // binary ���� ����
    }
    fclose(context_table_FILE);




    /* get_binary(table.hbs)�� �̿��� �ƽ�Ű code ���� code decode */
    map<int, string> table;
    string first = "";                        // 8 bit , Ascii code
    string second = "";                        // 8 bit , convert code length
    string third = "";                        // convert code
    while (1) {
        if (count == 129) {
            stuffing_ = stuffing_ + get_binary.substr(0, 8);
            break;
        }
        bitset<8> fix_table_code(count);
        table.insert(pair<int, string>(count, fix_table_code.to_string()));
        get_binary.erase(0, 8);
        count++;
    }

    /* �� �ƽ�Ű �ڵ�� ��ȯ code�� �̿��ؼ� tree�� ���� */
    map<int, string>::iterator it;
    it = table.begin();

    for (int i = 0; i < table.size(); i++) {
        Huf_Tree* current_node = root;

        for (int j = 0; j < it->second.length(); j++) {      //for���� �ٵ��� �Ѱ��� �ƽ�Ű�ڵ尪�� tree�� node�� ������ ��

           // ��ȯ�ڵ尡 0�̰� �̵� �����ҋ�
            if (it->second[j] == '0' && current_node->LeftNode != NULL) {
                current_node = current_node->LeftNode;
            }

            // ��ȯ �ڵ尡 0�̰� �̵� �Ұ��� ��
            else if (it->second[j] == '0' && current_node->LeftNode == NULL) {
                Huf_Tree* new_node = new Huf_Tree();      //�̵��� ��带 ����� �̵�
                new_node->ParentNode = current_node;
                current_node->LeftNode = new_node;
                current_node = new_node;
            }

            // ��ȯ �ڵ尡 1�̰� �̵� ������ �� 
            else if (it->second[j] == '1' && current_node->RightNode != NULL) {
                current_node = current_node->RightNode;
            }

            //��ȯ �ڵ尡 1�̰� �̵� �Ұ��Ҷ�
            else if (it->second[j] == '1' && current_node->RightNode == NULL) {
                Huf_Tree* new_node = new Huf_Tree();   //�̵��� ��带 ����� �̵�
                new_node->ParentNode = current_node;
                current_node->RightNode = new_node;
                current_node = new_node;
            }

            else {
                printf("error");
                return 0;
            }

            //��ȯ �ڵ� ������ �������� �̵��� �Ϸ�Ǿ�, �ش� ��忡 �ڽ��� �ƽ�Ű �ڵ�, ��ȯ �ڵ带 ����
            if (j == it->second.length() - 1) {
                current_node->ascy = it->first;
                current_node->code = it->second;
            }
        }
        it++;      //���� �ƽ�Ű �� Ʈ���� ������ ��
    }



    /* context tree ���� */

    map<int, Huf_Tree*> context_trees = make_context_trees(context_table_binary);

    /* context tree ���� �Ϸ� */


    for (int i = 0; i < 4; i++) {
        decoder_code_hbs(i, root, context_trees);
    }

    return 0;
}