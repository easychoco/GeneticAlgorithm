import numpy as np
import copy

from . import group

class GA:
  def __init__(self):
    # ----- 朝会問題のパラメータ -----
    # 朝会に参加する人数
    self.size = 44
    # 1グループあたりの最少人数
    self.min_group_member = 3
    # 計算に入れる日数
    self.day = 4

    # ----- GA のパラメータ -----
    # 各世代のエリートを出力する
    self.is_verbose = False
    # 個体群のサイズ
    self.population_size = 50
    # 世代数
    self.generation = 200
    # 交叉率
    self.crossover_rate = 0.5
    # 突然変異率
    self.mutation_rate = 0.5
    # リーダー交代処理の実行率
    self.roll_mutation_rate = 0.4
    # 突然変異の swap をする回数
    # n C 2 の 1 / 4 くらい
    self.swap_mutation_count = (self.size ** 2) // 8
    # トーナメント選択のトーナメントサイズ
    self.tournament_size = 3
    # 次の世代に紛れ込ませる初期個体の数
    self.insert_intial_num = 5

    # ----- 内部で使う用の変数 -----
    # エリート個体 (個地群の中で適応度が最も高い個体)
    self._elite = None
    # エリート個体の適応度
    self._elite_fitness = 0.0
    # 個体群
    self._population = []

  def run(self):

    # 初期個体群を作る
    for _ in range(self.population_size):
      self._population.append(self.create_individual())

    # 世代をループする
    for g in range(self.generation):
      print(f'generation {g + 1} / {self.generation} : {self._elite_fitness}')

      self.crossover()    # 交叉
      self.mutation()     # 突然変異
      self.update_elite() # エリートを更新
      self.selection()    # 次世代の個体を選択

      if (self.is_verbose):
        self._elite.show()
        print(f'elite_fitness is {self._elite_fitness}')
        print()

    for g in self._population: # validateion
      g.validate()

    print('completed.')
    self._elite.show_detail()

  # 個体をひとつ作る
  def create_individual(self):
    individual = group.Group(self.size, self.min_group_member, self.day)
    individual.build()
    return individual

  # 交叉
  def crossover(self):
    # 一点交叉
    def onePointCrossover(self, ind1, ind2):
      # cp = cut_point
      cp = np.random.randint(1, self.day)
      ret1 = ind1[0:cp]
      ret2 = ind2[0:cp]
      ret1.extend(ind2[cp::])
      ret2.extend(ind1[cp::])
      return ret1, ret2

    # 上から二つずつ交叉させる
    for i in range(self.population_size // 2):
      ind1 = self._population[i * 2].groups
      ind2 = self._population[i * 2 + 1].groups

      if np.random.rand() < self.crossover_rate:
        ind1, ind2 = onePointCrossover(self, ind1, ind2)
        self._population[i * 2].groups = ind1
        self._population[i * 2 + 1].groups = ind2

  # 突然変異
  def mutation(self):
    # グループをまたいで交換
    def swapMutation(today):
      for _ in range(self.swap_mutation_count):
        i1 = np.random.randint(0, self.size)
        i2 = np.random.randint(0, self.size)
        today[i1], today[i2] = today[i2], today[i1]
      return today

    # グループ内でリーダーを交代
    def rollMutation(today):
      for g in today:
        if np.random.rand() < self.roll_mutation_rate:
          next_g = g[1::]
          next_g.extend(g[0:1])
          g = next_g
      return today

    for ind in self._population:
      if np.random.rand() < self.mutation_rate:
        for today in ind.group_base:
          today = swapMutation(today)

        for today in ind.groups:
          today = rollMutation(today)
      
  # 選択
  def selection(self):
    # トーナメント選択
    # N 人の中で、一番適応度の高い個体が選ばれる
    def tournamentSelection():
      winner = None
      for _ in range(self.tournament_size):
        index = np.random.randint(self.population_size)
        candidates = self._population[index]
        if winner is None:
          winner = candidates
        elif winner.fitness < candidates.fitness:
          winner = candidates
      return winner

    next_population = []
    next_population.append(copy.deepcopy(self._elite))

    # 初期個体を紛れ込ませる
    for _ in range(self.insert_intial_num):
      next_population.append(self.create_individual())

    # 選択
    while len(next_population) < self.population_size:
      next_population.append(tournamentSelection())
    self._population = next_population

  # 適応度を更新して、エリート個体を決める    
  def update_elite(self):
    # 適応度を取得 / ソート用
    def get_fitness(ind):
      return ind.fitness

    # 交叉と突然変異の結果を適応して、適応度を計算する
    for ind in self._population:
      ind.build()
      ind.calc_fitness()

    # 適応度の降順でソート
    self._population.sort(key=get_fitness, reverse=True)

    # エリート個体を更新
    if self._elite_fitness < self._population[0].fitness:
      self._elite = copy.deepcopy(self._population[0])
      self._elite_fitness = self._elite.fitness
