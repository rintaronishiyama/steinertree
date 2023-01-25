#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include <chrono>
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
using std::stoi;
using std::to_string;

namespace fs = std::filesystem;
namespace ch = std::chrono;

int main(int argc, char* argv[])
{
    /* グラフの選択 */
    string graph_name;
    cout << "Enter graph name : ";
    cin >> graph_name;


    /* 避ける上位ノードの個数の間隔を +1 or +0.1% + ×2 から選択*/
    string sketch_interval;
    cout << "select sketch interval" << endl;
    cout << "1 : increment" << endl;
    cout << "2 : add0.1%" << endl;
    cout << "3 : multiply2" << endl;
    cin >> sketch_interval;
    if (sketch_interval != "1" && sketch_interval != "2" && sketch_interval != "3") {
        cout << "please slect 1 or 2 or 3" << endl;
        return 1;
    }
    if (sketch_interval == "1") {
        sketch_interval = "increment";
    }
    if (sketch_interval == "2") {
        sketch_interval = "add";
    }
    if (sketch_interval == "3") {
        sketch_interval = "multiply";
    }


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
    string result_dir_path = "./" + graph_name + "/" + sketch_interval;
    string dc_result_dir_path = result_dir_path + "/DC";
    string bc_result_dir_path = result_dir_path + "/BC";
    string cc_result_dir_path = result_dir_path + "/CC";

    string dc_extended_sketches_path = dc_result_dir_path + "/extended_sketches.txt";
    string bc_extended_sketches_path = bc_result_dir_path + "/extended_sketches.txt";
    string cc_extended_sketches_path = cc_result_dir_path + "/extended_sketches.txt";

    if ( !fs::is_regular_file(dc_extended_sketches_path) ) {
        cout << "There is no extended_sketches.txt for DC" << endl;
        return 1;
    }
    if ( !fs::is_regular_file(bc_extended_sketches_path) ) {
        cout << "There is no extended_sketches.txt for BC" << endl;
        return 1;
    }
    if ( !fs::is_regular_file(cc_extended_sketches_path) ) {
        cout << "There is no extended_sketches.txt for CC" << endl;
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
        string terminals_path = "./" + graph_name + "/terminals" + to_string(i) + ".txt";

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


    /* x軸の値のリストを取得 */
    // 外側 避けた上位ノードの種類 [dc, bc, cc]
    // 内側 避けた上位ノードの個数 [original, 1, 2, 4, ...]
    vector<vector<string> > list_of_list_of_x;

    string dc_x_list_path = dc_result_dir_path + "/x_list.txt";
    string bc_x_list_path = bc_result_dir_path + "/x_list.txt";
    string cc_x_list_path = cc_result_dir_path + "/x_list.txt";

    vector<string> dc_x_list;
    vector<string> bc_x_list;
    vector<string> cc_x_list;

    read_x_list(dc_x_list_path, dc_x_list);
    read_x_list(bc_x_list_path, bc_x_list);
    read_x_list(cc_x_list_path, cc_x_list);

    list_of_list_of_x.push_back(dc_x_list);
    list_of_list_of_x.push_back(bc_x_list);
    list_of_list_of_x.push_back(cc_x_list);
    cout << "Complete getting x list" << endl;


    /* extended sketches から sketches のリストを取得 */
    using Sketch = vector<vector<int> >;
    using Sketches = unordered_map<int, Sketch>;

    // [dc上位を避けたsketchesのリスト, bc上位を避けたsketchesのリスト, cc上位を避けたsketchesのリスト]
    vector<vector<Sketches> > list_of_list_of_sketches;

    for (int i = 0; i < extended_sketches_list.size(); ++i) {
        // 中心性上位を避けた sketches のリスト
        vector<Sketches> list_of_sketches
            = get_list_of_sketches_from_extended_sketches( extended_sketches_list.at(i), list_of_list_of_x.at(i) );
        
        list_of_list_of_sketches.push_back(list_of_sketches);
    }
    cout << "Complete getting list of avoided sketches" << endl;


    /* 評価の値のリスト */
    // 外 ターミナルの個数      [3, 4, 5, 6, 7]
    // 中 避けた上位ノードの種類 [dc, bc, cc]
    // 内 避けた上位ノードの個数 [|A0|, |A1|, |A2|, |A3|, ...]
    vector<vector<vector<double> > > threeDlist_of_overlap_ratio;
    vector<vector<vector<double> > > threeDlist_of_ST_size;
    vector<vector<vector<double> > > threeDlist_of_ST_size_ratio;
    vector<vector<vector<double> > > threeDlist_of_sum_of_dc;
    vector<vector<vector<double> > > threeDlist_of_sum_of_bc;
    vector<vector<vector<double> > > threeDlist_of_sum_of_cc;
    vector<vector<vector<double> > > threeDlist_of_sum_of_dc_ratio;
    vector<vector<vector<double> > > threeDlist_of_sum_of_bc_ratio;
    vector<vector<vector<double> > > threeDlist_of_sum_of_cc_ratio;
    vector<vector<vector<double> > > threeDlist_of_execution_time;

    for (int i = 0; i < list_of_list_of_terminals.size(); ++i) {
        vector<vector<double> > list_of_list_of_evaluation;

        for (const vector<string>& x_list : list_of_list_of_x) {
            vector<double> list_of_evaluation(x_list.size(), 0);
            list_of_list_of_evaluation.push_back(list_of_evaluation);
        }

        threeDlist_of_overlap_ratio.push_back(list_of_list_of_evaluation);
        threeDlist_of_ST_size.push_back(list_of_list_of_evaluation);
        threeDlist_of_ST_size_ratio.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_dc.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_bc.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_cc.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_dc_ratio.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_bc_ratio.push_back(list_of_list_of_evaluation);
        threeDlist_of_sum_of_cc_ratio.push_back(list_of_list_of_evaluation);
        threeDlist_of_execution_time.push_back(list_of_list_of_evaluation);
    }


    /* DC, BC, CC の読み込み */
    string DC_txt_path = "./" + graph_name + "/DC.txt";
    string BC_txt_path = "./" + graph_name + "/BC.txt";
    string CC_txt_path = "./" + graph_name + "/CC.txt";

    unordered_map<int, double> dc_map;
    unordered_map<int, double> bc_map;
    unordered_map<int, double> cc_map;

    read_centrality_from_txt_file(DC_txt_path, dc_map);
    read_centrality_from_txt_file(BC_txt_path, bc_map);
    read_centrality_from_txt_file(CC_txt_path, cc_map);


    /* ST 確認の為の準備 */
    // 外 ターミナルの個数      [3, 4, 5, 6, 7]
    // 内 避けた上位ノードの種類 [dc, bc, cc]
    vector<vector<Graph> > list_of_list_of_ST_for_checking;
    // ターミナルの個数 [3, 4, 5, 6, 7]
    vector<vector<int> > list_of_terminals_for_checking;


    /* グラフのノード数を取得 */
    int n = graph.get_number_of_nodes();


    /* 時間計測のための準備 */
    ch::system_clock::time_point start, end;


    /* 実行 */


    // for i [ターミナル数 3 個のターミナルリスト, ターミナル数 4 個のターミナルリスト, ...]
    // for j [bc, dc, cc]
    // for k [original, 上位1個避け, 上位2個避け, 上位4個避け, ...]
    for (int i = 0; i < list_of_list_of_terminals.size(); ++i) {
        cout << "terminals" << i + 3 << " start" << endl;




        // 並列化のための準備
        // 外側 ターミナル [1セット目, 2セット目, ...]
        // 中側 中心性指標 [dc, bc, cc]
        // 内側 中心性上位 [|A0|, |A1|, |A2|, ...]
        using ThreeDList = vector<vector<vector<double> > >;
        ThreeDList tmp_threeDlist_of_overlap_ratio;
        ThreeDList tmp_threeDlist_of_ST_size;
        ThreeDList tmp_threeDlist_of_ST_size_ratio;
        ThreeDList tmp_threeDlist_of_sum_of_dc;
        ThreeDList tmp_threeDlist_of_sum_of_bc;
        ThreeDList tmp_threeDlist_of_sum_of_cc;
        ThreeDList tmp_threeDlist_of_sum_of_dc_ratio;
        ThreeDList tmp_threeDlist_of_sum_of_bc_ratio;
        ThreeDList tmp_threeDlist_of_sum_of_cc_ratio;
        ThreeDList tmp_threeDlist_of_execution_time;

        // 初期化
        for (int l = 0; l < list_of_list_of_terminals.at(i).size(); ++l) {
            vector<vector<double> > list_of_list_of_evaluation;

            for (const vector<string>& x_list : list_of_list_of_x) {
                vector<double> list_of_evaluation(x_list.size(), 0);
                list_of_list_of_evaluation.push_back(list_of_evaluation);
            }

            tmp_threeDlist_of_overlap_ratio.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_ST_size.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_ST_size_ratio.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_sum_of_dc.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_sum_of_bc.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_sum_of_cc.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_sum_of_dc_ratio.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_sum_of_bc_ratio.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_sum_of_cc_ratio.push_back(list_of_list_of_evaluation);
            tmp_threeDlist_of_execution_time.push_back(list_of_list_of_evaluation);
        }




        // #pragma omp parallel for
        for (int l = 0; l < list_of_list_of_terminals.at(i).size(); ++l ) {
            vector<int> terminals = list_of_list_of_terminals.at(i).at(l);

            // STのリストのリスト
            // 外 避けた上位ノードの種類 [bc, dc, cc]
            // 内 避けた上位ノードの個数 [original, 1, 2, 4, ...]
            vector<vector<Graph> > list_of_list_of_ST;

            // sketchLS 実行
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                vector<Graph> list_of_ST;

                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    start = ch::system_clock::now();
                    list_of_ST.push_back(sketchLS(graph, terminals, list_of_list_of_sketches.at(j).at(k)));
                    end   = ch::system_clock::now();

                    tmp_threeDlist_of_execution_time[l][j][k]
                        = static_cast<double>(ch::duration_cast<ch::milliseconds>(end - start).count());
                }

                list_of_list_of_ST.push_back(list_of_ST);
            }

            // overlap_ratio 記録
            const Graph& original_ST = list_of_list_of_ST.front().front();
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_overlap_ratio[l][j][k]
                        = evaluate_overlap_ratio(original_ST, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // サイズ記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_ST_size[l][j][k]
                        = list_of_list_of_ST.at(j).at(k).get_number_of_edges();
                }
            }

            // サイズの比率記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                int original_ST_size
                    = list_of_list_of_ST.at(j).at(0).get_number_of_edges();
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_ST_size_ratio[l][j][k]
                        = list_of_list_of_ST.at(j).at(k).get_number_of_edges() / static_cast<double>(original_ST_size);
                }
            }

            // dc の合計記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_sum_of_dc[l][j][k]
                        = evaluate_sum_of_centrality(dc_map, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // bc の合計記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_sum_of_bc[l][j][k]
                        = evaluate_sum_of_centrality(bc_map, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // cc の合計記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_sum_of_cc[l][j][k]
                        = evaluate_sum_of_centrality(cc_map, list_of_list_of_ST.at(j).at(k), terminals);
                }
            }

            // dc の合計の比率記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                double original_sum_of_centrality = evaluate_sum_of_centrality(dc_map, list_of_list_of_ST.at(j).at(0), terminals);
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_sum_of_dc_ratio[l][j][k]
                        = evaluate_sum_of_centrality(dc_map, list_of_list_of_ST.at(j).at(k), terminals)
                            / original_sum_of_centrality;
                }
            }

            // cc の合計の比率記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                double original_sum_of_centrality = evaluate_sum_of_centrality(cc_map, list_of_list_of_ST.at(j).at(0), terminals);
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_sum_of_cc_ratio[l][j][k]
                        = evaluate_sum_of_centrality(cc_map, list_of_list_of_ST.at(j).at(k), terminals)
                            / original_sum_of_centrality;
                }
            }

            // bc の合計の比率記録
            for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
                double original_sum_of_centrality = evaluate_sum_of_centrality(bc_map, list_of_list_of_ST.at(j).at(0), terminals);
                for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                    tmp_threeDlist_of_sum_of_bc_ratio[l][j][k]
                        = evaluate_sum_of_centrality(bc_map, list_of_list_of_ST.at(j).at(k), terminals)
                            / original_sum_of_centrality;
                }
            }

            // ST 確認のため
            if (l == 0) {
                vector<Graph> list_of_ST_for_checking;
                for (const vector<Graph>& list_of_ST : list_of_list_of_ST) {
                    list_of_ST_for_checking.push_back(list_of_ST.back());
                }

                list_of_list_of_ST_for_checking.push_back(list_of_ST_for_checking);
                list_of_terminals_for_checking.push_back(terminals);
            }
        }

        


        // 並列化のための後処理
        for (int j = 0; j < list_of_list_of_sketches.size(); ++j) {
            for (int k = 0; k < list_of_list_of_sketches.at(j).size(); ++k) {
                for (int l = 0; l < list_of_list_of_terminals.at(i).size(); ++l) {
                    threeDlist_of_overlap_ratio[i][j][k]  += tmp_threeDlist_of_overlap_ratio[l][j][k];
                    threeDlist_of_ST_size[i][j][k]        += tmp_threeDlist_of_ST_size[l][j][k];
                    threeDlist_of_ST_size_ratio[i][j][k]  += tmp_threeDlist_of_ST_size_ratio[l][j][k];
                    threeDlist_of_sum_of_dc[i][j][k]      += tmp_threeDlist_of_sum_of_dc[l][j][k];
                    threeDlist_of_sum_of_bc[i][j][k]      += tmp_threeDlist_of_sum_of_bc[l][j][k];
                    threeDlist_of_sum_of_cc[i][j][k]      += tmp_threeDlist_of_sum_of_cc[l][j][k];
                    threeDlist_of_sum_of_dc_ratio[i][j][k]+= tmp_threeDlist_of_sum_of_dc_ratio[l][j][k];
                    threeDlist_of_sum_of_bc_ratio[i][j][k]+= tmp_threeDlist_of_sum_of_bc_ratio[l][j][k];
                    threeDlist_of_sum_of_cc_ratio[i][j][k]+= tmp_threeDlist_of_sum_of_cc_ratio[l][j][k];
                    threeDlist_of_execution_time[i][j][k] += tmp_threeDlist_of_execution_time[l][j][k];
                }
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

    for (int i = 0; i < threeDlist_of_ST_size_ratio.size(); ++i) {
        for (int j = 0; j < threeDlist_of_ST_size_ratio.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_ST_size_ratio.at(i).at(j).size(); ++k) {
                threeDlist_of_ST_size_ratio[i][j][k] /= list_of_list_of_terminals.at(i).size();
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

    for (int i = 0; i < threeDlist_of_sum_of_dc_ratio.size(); ++i) {
        for (int j = 0; j < threeDlist_of_sum_of_dc_ratio.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_sum_of_dc_ratio.at(i).at(j).size(); ++k) {
                threeDlist_of_sum_of_dc_ratio[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }

    for (int i = 0; i < threeDlist_of_sum_of_bc_ratio.size(); ++i) {
        for (int j = 0; j < threeDlist_of_sum_of_bc_ratio.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_sum_of_bc_ratio.at(i).at(j).size(); ++k) {
                threeDlist_of_sum_of_bc_ratio[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }

    for (int i = 0; i < threeDlist_of_sum_of_cc_ratio.size(); ++i) {
        for (int j = 0; j < threeDlist_of_sum_of_cc_ratio.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_sum_of_cc_ratio.at(i).at(j).size(); ++k) {
                threeDlist_of_sum_of_cc_ratio[i][j][k] /= list_of_list_of_terminals.at(i).size();
            }
        }
    }

    for (int i = 0; i < threeDlist_of_execution_time.size(); ++i) {
        for (int j = 0; j < threeDlist_of_execution_time.at(i).size(); ++j) {
            for (int k = 0; k < threeDlist_of_execution_time.at(i).at(j).size(); ++k) {
                threeDlist_of_execution_time[i][j][k] /= list_of_list_of_terminals.at(i).size();
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
        for (int j = 0; j < list_of_list_of_terminals.size(); ++j) {
            // ターミナルの個数に応じたディレクトリを用意
            string terminal_result_dir_path = result_dir_path_list.at(i) + "/terminals" + to_string(j + 3);
            fs::create_directories(terminal_result_dir_path);

            string overlap_ratio_path = terminal_result_dir_path + "/overlap_ratio.txt";
            string ST_size_path       = terminal_result_dir_path + "/ST_size.txt";
            string ST_size_ratio_path       = terminal_result_dir_path + "/ST_size_ratio.txt";
            string sum_of_dc_path     = terminal_result_dir_path + "/sum_of_DC.txt";
            string sum_of_bc_path     = terminal_result_dir_path + "/sum_of_BC.txt";
            string sum_of_cc_path     = terminal_result_dir_path + "/sum_of_CC.txt";
            string sum_of_dc_ratio_path     = terminal_result_dir_path + "/sum_of_DC_ratio.txt";
            string sum_of_bc_ratio_path     = terminal_result_dir_path + "/sum_of_BC_ratio.txt";
            string sum_of_cc_ratio_path     = terminal_result_dir_path + "/sum_of_CC_ratio.txt";
            string execution_time_path= terminal_result_dir_path + "/execution_time.txt";

            write_evaluation(
                overlap_ratio_path,
                list_of_list_of_x.at(i),
                threeDlist_of_overlap_ratio.at(j).at(i) );

            write_evaluation(
                ST_size_path,
                list_of_list_of_x.at(i),
                threeDlist_of_ST_size.at(j).at(i) );

            write_evaluation(
                ST_size_ratio_path,
                list_of_list_of_x.at(i),
                threeDlist_of_ST_size_ratio.at(j).at(i) );

            write_evaluation(
                sum_of_dc_path,
                list_of_list_of_x.at(i),
                threeDlist_of_sum_of_dc.at(j).at(i) );

            write_evaluation(
                sum_of_bc_path,
                list_of_list_of_x.at(i),
                threeDlist_of_sum_of_bc.at(j).at(i) );

            write_evaluation(
                sum_of_cc_path,
                list_of_list_of_x.at(i),
                threeDlist_of_sum_of_cc.at(j).at(i) );
            
            write_evaluation(
                sum_of_dc_ratio_path,
                list_of_list_of_x.at(i),
                threeDlist_of_sum_of_dc_ratio.at(j).at(i) );

            write_evaluation(
                sum_of_bc_ratio_path,
                list_of_list_of_x.at(i),
                threeDlist_of_sum_of_bc_ratio.at(j).at(i) );

            write_evaluation(
                sum_of_cc_ratio_path,
                list_of_list_of_x.at(i),
                threeDlist_of_sum_of_cc_ratio.at(j).at(i) );
                
            write_evaluation(
                execution_time_path,
                list_of_list_of_x.at(i),
                threeDlist_of_execution_time.at(j).at(i) );
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
        for (int j = 0; j < list_of_list_of_terminals.size(); ++j) {
            const Graph& ST = list_of_list_of_ST_for_checking.at(j).at(i);
            const vector<int>& terminals = list_of_terminals_for_checking.at(j);

            string ST_path = ST_checking_path + "/ST" + to_string(j + 3) + ".txt";

            write_graph(ST_path, ST);
            write_terminals_to_exisiting_txt(ST_path, terminals);
        }
    }
    cout << "Complete writing STs for checking" << endl;


    return 0;
}