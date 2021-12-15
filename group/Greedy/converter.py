import csv

csv_file = open("./in.tsv", "r", encoding="utf-8", errors="", newline="" )
#リスト形式
in_tsv = csv.reader(csv_file, delimiter="\t", doublequote=True, lineterminator="\r\n", quotechar='"', skipinitialspace=True)

body = [' '.join(row[2:]) for row in in_tsv]

n = len(body) # 全員の人数
m = 3 # 何人のグループをつくるか
cnt = body[0].count(' ') + 1 # 入力の列数 / もうちょっとうまくできないかな

with open('in', 'w') as writer:
  writer.write(f'{n} {m} {cnt}\n')
  writer.write('\n'.join(body))
