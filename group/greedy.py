#!/home/gummy/.local/share/virtualenvs/dl-keras-6cZ1M_Ap/bin/python3

import Greedy

def main():
  group = Greedy.group.Group()

  group.make_group()

  group.validate()

  ed = group.evaluate_duplicate()
  el = group.evaluate_leader()
  print(ed, el)

  group.show()

if __name__ == '__main__':
  main()
