import numpy as np

class Group:

  # group[i][j][k] = i 日目の j 番目のグループに k の人が含まれる
  groups = []
  # group_base[i][j] = i 日目のメンバー / 各グループに振り分ける前の状態
  group_base = None
  # 適応度
  fitness = 0.0

  def __init__(self, n = 10, m = 3, day = 5):
    self.n = n # n : 全体の人数
    self.m = m # m : グループの人数
    self.day = day # day : 作る日
    self.group_num = n // m # グループ数
    # group を作るもと
    # group_base[i][j] = i 日目の j 番目人の番号
    self.group_base = np.tile(np.arange(n), (day, 1))

    # 各グループの人数
    self.each_group_size = np.full(self.group_num, self.m)
    for i in range(self.n % self.group_num):
      self.each_group_size[i] += 1
    self.build()
    
  # グループを出力する
  def show(self):
    ret = np.array(self.groups, dtype=object)
    print(ret)

  # グループの詳細を出力する
  def show_detail(self):
    for g in self.group_base:
      print(g)
    print(f'fitness: {self.fitness}')
    print(f'メンバーの重複: {self.evaluate_duplicate()}')
    print(f'リーダーの偏り: {self.evaluate_leader() - 1}')

  # group_base から groups を作る
  # each_group_size の人数ごとに分配する
  def build(self):
    self.groups = [ [ [] for _ in range(self.group_num) ] for _ in range(self.day) ]
    for today in range(self.day):
      now_idx = 0
      for i, sz in enumerate(self.each_group_size):
        for _ in range(sz):
          self.groups[today][i].append(self.group_base[today][now_idx])
          now_idx += 1

  # グループの形があっているか確認
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
      
      # 日数を validate
      err_day_count = 'err_day_count'
      # グループの人数を validate
      err_group_size = 'err_group_size'
      # メンバーの重複を validate
      err_member_duplicate = 'err_member_duplicate'

      try:
        if len(self.groups) != self.day:
          print(f'day is {self.day} but {len(self.groups)}')
          raise ValueError(err_day_count)
        if max_v - min_v > 1:
          print(f'min: {min_v}, max: {max_v}')
          raise ValueError(err_group_size)
        for i in range(self.n):
          if bin[i] > 1:
            print(f'idx: {i} is {bin[i]}')
            raise ValueError(err_member_duplicate)
      except ValueError as e:
        print(e)
        exit()

  # 日時経過によるメンバー間の重複を評価
  def evaluate_duplicate(self):
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

  # リーダーの偏りを評価
  def evaluate_leader(self):
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

  # 適応度を計算して更新する
  def calc_fitness(self):
    self.fitness = 1.0 / max(1.0, self.evaluate_duplicate() + self.evaluate_leader() * self.n)
    return self.fitness


