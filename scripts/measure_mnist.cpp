#include <iostream>
#include <fstream>
#include <stdlib.h>

void GenTops(int tops[][30], int ground_truth[]) {
  int top1 = 0;
  int top5 = 0;
  for (int i = 0; i < 30; i++) {
    if (tops[0][i] == ground_truth[i]) {
      top1++;
      top5++;
    } else if (tops[1][i] == ground_truth[i] ||
               tops[2][i] == ground_truth[i] ||
               tops[3][i] == ground_truth[i] ||
               tops[4][i] == ground_truth[i]) {
      top5++;
    }
  }

  std::cout << "Top1: "  << ((float)top1/30) * 100 << "%\n";
  std::cout << "Top5: "  << ((float)top5/30) * 100 << "%\n";
}

void GenConfusionMetrics(int top1[], int ground_truth[]) {
  int total_predicted_positive[30];
  int total_actual_positive[30];
  int M[30][30];
  int has_class[30];
  float precision[30];
  float recall[30];

  for (int i = 0; i < 30; i++) {
    has_class[i] = 0;
    total_predicted_positive[i] = 0;
    total_actual_positive[i] = 0;
    for (int j = 0; j < 30; j++) {
      M[i][j] = 0;
    }
  }

  for (int i = 0; i < 30; i++) {
    int y = ground_truth[i]; 
    int x = top1[i]; 
    M[y][x]++;
    has_class[y] = 1;
  }

  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < 30; j++) {
      total_predicted_positive[i] += M[j][i];
      total_actual_positive[i] += M[i][j];
    }
  }

  int num_classes = 30;
  for (int i = 0; i < 30; i++) {
    int val = M[i][i];
    
    if (total_predicted_positive[i] == 0 || total_actual_positive[i] == 0) {
      precision[i] = 0;
      recall[i] = 0;
      if (has_class[i] == 0) {
        num_classes--;
      }
    } else {
      precision[i] = (float)val / total_predicted_positive[i];
      recall[i] = (float)val / total_actual_positive[i];
    }
  }

  float final_precision = 0.0;
  float final_recall = 0.0;

  for (int i = 0; i < 30; i++) {
    final_precision += precision[i];
    final_recall += recall[i];
  }

  final_precision = final_precision / num_classes;
  final_recall = final_recall / num_classes;

  float f1_score = 2 * ((final_precision * final_recall) / (final_precision + final_recall));

  std::cout << "Precision: " << final_precision*100 << "%\n";
  std::cout << "Recall: " << final_recall*100 << "%\n";
  std::cout << "F1-score: " << f1_score*100 << "%\n";
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    std::cout << "Use: ./program [FILE_RES] [GROUND_TRUTH] \n";
    exit(1);
  }

  std::ifstream infile;
  std::string file_name[30];
  std::string confidence[30];
  float precision[30];

  int tops[5][30];
  int ground_truth[30];

  // Open the result of a inference
  int num = 0;
  infile.open((argv[1]));
  if(infile.fail())
  {
    std::cout << "error" << std::endl;
    return 1;
  }
  while(!infile.eof())
  {
    infile >> file_name[num]; 
    infile >> tops[0][num];
    infile >> tops[1][num];
    infile >> tops[2][num];
    infile >> tops[3][num];
    infile >> tops[4][num];
    infile >> confidence[num];
    infile >> precision[num];
    ++num;
  }
  infile.close();

  // Open ground thruth
  num = 0;
  infile.open(argv[2]);
  if(infile.fail())
  {
    std::cout << "error" << std::endl;
    return 1;
  }
  while(!infile.eof())
  {
    infile >> ground_truth[num];
    ++num;
  }
  infile.close();

  GenTops(tops, ground_truth);

  GenConfusionMetrics(tops[0], ground_truth);

  return 0; // everything went right.
}
