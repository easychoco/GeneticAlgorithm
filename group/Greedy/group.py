import numpy as np

class Group:

  # group[i][j][k] = i 日目の j 番目のグループに k の人が含まれる
  group = []

  def __init__(self, n = 10, m = 3, day = 5):
    self.n = n # n : 全体の人数
    self.m = m # m : グループの人数
    self.day = day # day : 作る日
    self.group_num = n // m # グループ数
    # group を作るもと
    # group_base[i][j] = i 日目の j 番目人の番号
    self.group_base = np.tile(np.arange(n), (day, 1))

  def show(self):
    print(f'n: {self.n}, m: {self.m}, day: {self.day}')
    ret = np.array(self.groups, dtype=object)
    print(ret,)

  def make_group(self):
    # ここで group_base に諸々の演算をする

    # これ以降で、各グループに分配する

    # 各グループの人数
    each_group_size = np.full(self.group_num, self.m)
    for i in range(self.n % self.group_num):
      each_group_size[i] += 1
    
    # each_group_size の人数ごとに分配する
    self.groups = [ [ [] for _ in range(self.group_num) ] for _ in range(self.day) ]
    for today in range(self.day):
      now_idx = 0
      for i, sz in enumerate(each_group_size):
        for _ in range(sz):
          self.groups[today][i].append(self.group_base[today][now_idx])
          now_idx += 1

  def validate(self):
    for today in range(self.day):
      max_v = 0
      min_v = 1001001001
      bin = np.zeros(self.n)
      for g in self.groups[today]:
        min_v = min(min_v, len(g))
        max_v = max(max_v, len(g))
        for v in g:
          bin[v] += 1
      
      # グループの人数を validate
      err_group_size = 'err_group_size'
      # メンバーの重複を validate
      err_member_duplicate = 'err_member_duplicate'

      try:
        if max_v - min_v > 1:
          print(f'min: {min_v}, max: {max_v}')
          raise ValueError(err_group_size)
        for i in range(self.n):
          if bin[i] > 1:
            print(f'idx: {i} is {bin[i]}')
            raise ValueError(err_member_duplicate)
      except ValueError as e:
        print(e)

  def evaluate_duplicate(self):
    # 日時経過によるメンバー間の重複を評価
    ret = 0
    check = np.zeros(self.n, dtype=np.int64)
    for i in range(self.n):
      check[i] = (1 << i)

    for group in self.groups:
      for members in group:
        for me in members: # 自分
          for pair in members: # 相手
            if me == pair: continue
            if (check[me] >> pair) & 1:
              ret += 1
            check[me] |= (1 << pair)
    return ret

  def evaluate_leader(self):
    # リーダーの偏りを評価
    leader_count = np.zeros(self.n, dtype=np.int32)

    for group in self.groups:
      for members in group:
        if len(members) == 0: continue
        leader_count[members[0]] += 1

    max_v = 0
    min_v = 1001001001
    for group in self.groups:
      for members in group:
        for member in members:
          min_v = min(min_v, leader_count[member])
          max_v = max(max_v, leader_count[member])

    return max_v - min_v
