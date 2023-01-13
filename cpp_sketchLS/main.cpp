#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include "graph.h"
#include "read.h"
#include "random.h"
#include "sketchLS.h"
#include "write.h"
#include "divide.h"
#include "evaluate.h"
#include "x.h"

// Files to compile
// main.cpp graph.cpp read.cpp split.cpp sketchLS.cpp write.cpp random.cpp divide.cpp evaluate.cpp x.cpp

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::unordered_map;
using std::pair;
using std::to_string;

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    /* グラフの選択 */
    string graph_name;
    cout << "Enter graph name : ";
    cin >> graph_name;


    /* グラフのデータセットがあるか確認 */
    string dataset_path = "../dataset/" + graph_name + ".txt";
    if ( !fs::is_regular_file(dataset_path) ) { // なければ異常終了
        cout << "There is no such dataset" << endl;
        return 1;
    }


    /* グラフ読み込み */
    Graph graph;
    read_graph_from_txt_file(dataset_path, graph);
    cout << "Complete reading graph" << endl;
    

    /* extended_sketches が生成済みか確認 */
    string result_dir_path = "./" + graph_name;
    string dc_result_dir_path = "./" + graph_name + "/dc";
    string bc_result_dir_path = "./" + graph_name + "/bc";
    string cc_result_dir_path = "./" + graph_name + "/cc";

    string dc_extended_sketches_path = dc_result_dir_path + "/extended_sketches.txt";
    string bc_extended_sketches_path = bc_result_dir_path + "/extended_sketches.txt";
    string cc_extended_sketches_path = cc_result_dir_path + "/extended_sketches.txt";

    if ( !fs::is_regular_file(dc_extended_sketches_path) ) {
        cout << "There is no extended_sketches.txt for dc" << endl;
        return 1;
    }
    if ( !fs::is_regular_file(bc_extended_sketches_path) ) {
        cout << "There is no extended_sketches.txt for bc" << endl;
        return 1;
    }
    if ( !fs::is_regular_file(cc_extended_sketches_path) ) {
        cout << "There is no extended_sketches.txt for cc" << endl;
        return 1;
    }


    /* extended_sketches 読み込み */
    // 型エイリアス
    using Path_List = vector<vector<int> >;
    using Extended_Sketch = vector<Path_List>;
    using Extended_Sketches = unordered_map<int, Extended_Sketch>;

    // extended_sketches のリスト [dc_extended_sketches, bc_extended_sketches, cc_extended_sketches]
    vector<Extended_Sketches> extended_sketches_list;

    Extended_Sketches dc_extended_sketches;
    Extended_Sketches bc_extended_sketches;
    Extended_Sketches cc_extended_sketches;

    read_extended_sketches_from_txt_file(dc_extended_sketches_path, dc_extended_sketches);
    read_extended_sketches_from_txt_file(bc_extended_sketches_path, bc_extended_sketches);
    read_extended_sketches_from_txt_file(cc_extended_sketches_path, cc_extended_sketches);

    extended_sketches_list.push_back(dc_extended_sketches);
    extended_sketches_list.push_back(bc_extended_sketches);
    extended_sketches_list.push_back(cc_extended_sketches);
    cout << "Complete reading extended sketches" << endl;


    /* ターミナルファイルが生成済みか確認 */
    vector<string> terminals_path_list;
    int min_num_of_terminals = 3;
    int max_num_of_terminals = 7;

    for (int i = min_num_of_terminals; i <= max_num_of_terminals; ++i) {
        string terminals_path = result_dir_path + "/terminals" + to_string(i) + ".txt";

        if ( !fs::is_regular_file(terminals_path) ) {
            cout << "There is no terminals" << i << ".txt" << endl;
            return 1;
        }

        terminals_path_list.push_back(terminals_path);
    }


    /* ターミナルを読み込み */
    // ターミナルの個数 [3, 4, 5, 6, 7] 毎に 1000 セットの terminals
    vector<vector<vector<int> > > list_of_list_of_terminals;

    for (const string& terminals_path : terminals_path_list) {
        vector<vector<int> > list_of_terminals;

        read_list_of_terminals_from_txt_file(terminals_path, list_of_terminals);

        list_of_list_of_terminals.push_back(list_of_terminals);
    }
    cout << "Complete reading terminals" << endl;


    /* extended sketches から sketches のリストを取得 */
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;

    // [dc上位を避けたsketchesのリスト, bc上位を避けたsketchesのリスト, cc上位を避けたsketchesのリスト]
    vector<vector<Sketches> > list_of_list_of_sketches;

    // 避けて経路を生成できた[dc上位の個数の最大, bc上位の個数の最大, cc上位の個数の最大]
    vector<int> max_number_of_avoided_top_nodes_list;

    for (const Extended_Sketches& extended_sketches : extended_sketches_list) {
        // 避けて経路を生成できた上位の個数の最大
        int max_number_of_avoided_top_nodes = get_max_number_of_avoided_top_nodes(extended_sketches);
        max_number_of_avoided_top_nodes_list.push_back(max_number_of_avoided_top_nodes);
        
        // degree 上位を避けた sketches のリスト
        vector<Sketches> list_of_sketches
            = get_list_of_sketches_from_extended_sketches(extended_sketches, max_number_of_avoided_top_nodes);
        
        list_of_list_of_sketches.push_back(list_of_sketches);
    }
    cout << "Complete getting list of avoided sketches" << endl;


    /* 避けられた上位ノードの個数を表示 */
    cout << "max number of avoided top nodes" << endl;
    cout << "dc : " << max_number_of_avoided_top_nodes_list[0] << endl;
    cout << "bc : " << max_number_of_avoided_top_nodes_list[1] << endl;
    cout << "cc : " << max_number_of_avoided_top_nodes_list[2] << endl;


    /* x軸の値のリストを取得 */
    // 外側 避けた上位ノードの種類 [dc, bc, cc]
    // 内側 避けた上位ノードの個数 [original, 1, 2, 4, ...]
    vector<vector<string> > list_of_x_list;
    
    list_of_x_list.push_back(get_x_list_for_avoided_top_nodes(max_number_of_avoided_top_nodes_list[0]));
    list_of_x_list.push_back(get_x_list_for_avoided_top_nodes(max_number_of_avoided_top_nodes_list[1]));
    list_of_x_list.push_back(get_x_list_for_avoided_top_nodes(max_number_of_avoided_top_nodes_list[2]));
    cout << "Complete getting x list" << endl;


    /* 評価の値のリスト */
    // 外 ターミナルの個数      [3, 4, 5, 6, 7]
    // 中 避けた上位ノードの種類 [dc, bc, cc]
    // 内 避けた上位ノードの個数 [original, 1, 2, 4, ...]
    vector<vector<vector<double> > > threeDlist_of_overlap_ratio;
    vector<vector<vector<double> > > threeDlist_of_ST_size;
    vector<vector<vector<double> > > threeDlist_of_sum_of_dc;
    vector<vector<vector<double> > > threeDlist_of_sum_of_bc;
    vector<vector<vector<double> > > threeDlist_of_sum_of_cc;

    for (int i = 0; i < list_of_list_of_terminals.size(); ++i) {
        vector<vector<double> > list_of_list_of_evaluation;

        for (const vector<string>& x_list : list_of_x_list) {
            vector<double> list_of_evaluation(x_list.size(), 0);
            list_of_list_of_evaluation.push_back(list_of_evaluation);
        }

        threeDlist_of_overlap_ratio.push_back(list_of_list_of_evaluation);
        threeDlist_of_ST_size.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_dc.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_bc.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_cc.push_back(list_of_list_of_evaluation);
    }


    /* DC, BC, CC の読み込み */
    string DC_txt_path = "./" + graph_name + "/DC.txt";
    string BC_txt_path = "./" + graph_name + "/BC.txt";
    string CC_txt_path = "./" + graph_name + "/CC.txt";

    unordered_map<int, double> dc_map;
    unordered_map<int, double> bc_map;
    unordered_map<int, double> cc_map;

    read_bc_from_txt_file(DC_txt_path, dc_map);
    read_bc_from_txt_file(BC_txt_path, bc_map);
    read_bc_from_txt_file(CC_txt_path, cc_map);


    /* ST 確認の為の準備 */
    // 外 ターミナルの個数      [3, 4, 5, 6, 7]
    // 内 避けた上位ノードの種類 [dc, bc, cc]
    vector<vector<Graph> > list_of_list_of_ST_for_checking;
    // ターミナルの個数 [3, 4, 5, 6, 7]
    vector<vector<int> > list_of_terminals_for_checking;


    /* グラフのノード数を取得 */
    int n = graph.get_number_of_nodes();


    /* 実行 */


    // for i [ターミナル数 3 個のターミナルリスト, ターミナル数 4 個のターミナルリスト, ...]
    // for j [bc, dc, cc]
    // for k [original, 上位1個避け, 上位2個避け, 上位4個避け, ...]
    for (int i = 0; i < list_of_list_of_terminals.size(); ++i) {
        cout << i + 3 << "set start" << endl;

        int count_terminals = 0;
        for ( const vector<int>& terminals : list_of_list_of_terminals.at(i) ) {
            ++count_terminals;

            // STのリストのリスト
            // 外 避けた上位ノードの種類 [bc, dc, cc]
            // 内 避けた上位ノードの個数 [original, 1, 2, 4, ...]
            vector<vector<Graph> > list_of_list_of_ST;

            // sketchLS 実行
            for (const vector<Sketches>& list_of_sketches : list_of_list_of_sketches) {
                vector<Graph> list_of_ST;

                for (const Sketches& sketches : list_of_sketches) {
                    list_of_ST.push_back(sketchLS(graph, terminals, sketches));
                }

                list_of_list_of_ST.push_back(list_of_ST);
            }

            // overlap_ratio 記録
            const Graph& original_ST = list_of_list_of_ST.front().front();
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    threeDlist_of_overlap_ratio[i][j][k]
                        += evaluate_overlap_ratio(original_ST, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // サイズ記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    threeDlist_of_ST_size[i][j][k]
                        += list_of_list_of_ST.at(j).at(k).get_number_of_edges();
                }
            }

            // dc の合計記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    threeDlist_of_sum_of_dc[i][j][k]
                        += evaluate_sum_of_centrality(dc_map, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // bc の合計記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    threeDlist_of_sum_of_bc[i][j][k]
                        += evaluate_sum_of_centrality(bc_map, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // cc の合計記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    threeDlist_of_sum_of_cc[i][j][k]
                        += evaluate_sum_of_centrality(cc_map, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // ST 確認のため
            if (count_terminals == 1) {
                vector<Graph> list_of_ST_for_checking;
                for (const vector<Graph>& list_of_ST : list_of_list_of_ST) {
                    list_of_ST_for_checking.push_back(list_of_ST.back());
                }

                list_of_list_of_ST_for_checking.push_back(list_of_ST_for_checking);
                list_of_terminals_for_checking.push_back(terminals);
            }
        }
    }
    cout << "Complete executing" << endl;


    /* 平均化 */
    for (int i = 0; i < threeDlist_of_overlap_ratio.size(); ++i) {
        for (int j = 0; j < threeDlist_of_overlap_ratio.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_overlap_ratio.at(i).at(j).size(); ++k) {
                threeDlist_of_overlap_ratio[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }

    for (int i = 0; i < threeDlist_of_ST_size.size(); ++i) {
        for (int j = 0; j < threeDlist_of_ST_size.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_ST_size.at(i).at(j).size(); ++k) {
                threeDlist_of_ST_size[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }

    for (int i = 0; i < threeDlist_of_sum_of_dc.size(); ++i) {
        for (int j = 0; j < threeDlist_of_sum_of_dc.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_sum_of_dc.at(i).at(j).size(); ++k) {
                threeDlist_of_sum_of_dc[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }

    for (int i = 0; i < threeDlist_of_sum_of_bc.size(); ++i) {
        for (int j = 0; j < threeDlist_of_sum_of_bc.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_sum_of_bc.at(i).at(j).size(); ++k) {
                threeDlist_of_sum_of_bc[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }

    for (int i = 0; i < threeDlist_of_sum_of_cc.size(); ++i) {
        for (int j = 0; j < threeDlist_of_sum_of_cc.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_sum_of_cc.at(i).at(j).size(); ++k) {
                threeDlist_of_sum_of_cc[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }
    cout << "Complete averaging" << endl;


    /* 評価を保存 */
    vector<string> result_dir_path_list;
    result_dir_path_list.push_back(dc_result_dir_path);
    result_dir_path_list.push_back(bc_result_dir_path);
    result_dir_path_list.push_back(cc_result_dir_path);

    // for i [dc, bc, cc]
    // for j [3, 4, 5, 6, 7]
    for (int i = 0; i < result_dir_path_list.size(); ++i) {
        for (int j = min_num_of_terminals; j <= max_num_of_terminals; ++j) {
            // ターミナルの個数に応じたディレクトリを用意
            string terminal_result_dir_path = result_dir_path_list.at(i) + "/terminals" + to_string(j);
            fs::create_directories(terminal_result_dir_path);

            string overlap_ratio_path = terminal_result_dir_path + "/overlap_ratio.txt";
            string ST_size_path       = terminal_result_dir_path + "/ST_size.txt";
            string sum_of_dc_path     = terminal_result_dir_path + "/sum_of_dc.txt";
            string sum_of_bc_path     = terminal_result_dir_path + "/sum_of_bc.txt";
            string sum_of_cc_path     = terminal_result_dir_path + "/sum_of_cc.txt";

            write_evaluation(
                overlap_ratio_path,
                list_of_x_list.at(i),
                threeDlist_of_overlap_ratio.at(j).at(i) );

            write_evaluation(
                overlap_ratio_path,
                list_of_x_list.at(i),
                threeDlist_of_ST_size.at(j).at(i) );

            write_evaluation(
                overlap_ratio_path,
                list_of_x_list.at(i),
                threeDlist_of_sum_of_dc.at(j).at(i) );

            write_evaluation(
                overlap_ratio_path,
                list_of_x_list.at(i),
                threeDlist_of_sum_of_bc.at(j).at(i) );

            write_evaluation(
                overlap_ratio_path,
                list_of_x_list.at(i),
                threeDlist_of_sum_of_cc.at(j).at(i) );
        }
    }
    cout << "Complete writing evaluation" << endl;


    /* ST 確認のために保存 */
    // for i [dc, bc, cc]
    // for j [3, 4, 5, 6, 7]
    for (int i = 0; i < result_dir_path_list.size(); ++i) {
        // ディレクトリを用意
        string ST_checking_path = result_dir_path_list.at(i) + "/ST_checking";
        fs::create_directories(ST_checking_path);

        // ターミナル数毎のSTを保存
        for (int j = min_num_of_terminals; j <= max_num_of_terminals; ++j) {
            const Graph& ST = list_of_list_of_ST_for_checking.at(j).at(i);
            const vector<int>& terminals = list_of_terminals_for_checking.at(j);

            string ST_path = ST_checking_path + "/ST" + to_string(j) + ".txt";

            write_graph(ST_path, ST);
            write_terminals_to_exisiting_txt(ST_path, terminals);
        }
    }
    cout << "Complete writing STs for checking" << endl;


    return 0;
}