// C++ で書いた貪欲法のプログラム
// n人を mずつのグループに分ける
// n, m を入力で受け取る
#include <bits/stdc++.h>

#define fastio ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
#define endl "\n"
#define rep(i,n) repi(i,0,n)
#define repi(i,a,n) for(ll i=(a);i<(ll)(n);++i)
#define repe(i,n) repie(i,0,n)
#define repie(i,a,n) for(ll i=(a);i<=(ll)(n);++i)
#define rrep(i,n) rrepi(i,n,0)
#define rrepi(i,n,a) for(ll i=(n);i>=(a);--i)
#define ALL(a) (a).begin(),(a).end()
#define RALL(a) (a).rbegin(),(a).rend()

using namespace std;
using ll = int_fast64_t;
using vl = vector<ll>;
using vvl = vector<vl>;
using P = pair<ll, ll>;
using vp = vector<P>;
using vvp = vector<vp>;
const ll INF = 1LL << 60;
void YN(bool a) { cout << (a ? "Yes" : "No") << endl; }
template<class T> inline bool chmax(T& a, T b) { if (a < b) { a = b; return true; } return false; }
template<class T> inline bool chmin(T& a, T b) { if (a > b) { a = b; return true; } return false; }
void show(){ cout << endl; }
template <class Head, class... Tail>
void show(Head&& head, Tail&&... tail){ cout << head << " "; show(std::forward<Tail>(tail)...); }
template<class T> inline void showall(T& a) { for(auto v:a) cout<<v<<" "; cout<<endl; }

// 適応度
// 値が低いほど交流がないことを示す
ll calc_score(ll me, ll you, const vvl &history)
{
  // 同じグループになったことのある回数を返す
  return history[me][you];
}

// n : 全体の人数
// m : グループの人数
// day : 作る日
// history[i][j] : i と j が過去に同じチームになった回数
// @return group[i][j][k] = i 日目の j 番目のグループに k の人が含まれる
vvl make_group(ll n, ll m, const vvl &history, const vl &leader_num)
{
  // できるグループの数
  ll group_num = n / m;

  // ランダム性を持たせるために乱数を使う
  random_device seed_gen;
  mt19937 rand(seed_gen());

  // pair<score, random, me, you>
  vector<tuple<ll, ll, ll, ll>> all_pair;
  rep(i1, n)
  rep(i2, i1)
  {
    all_pair.push_back(make_tuple(
      calc_score(i1, i2, history),
      rand(),
      i1,
      i2
    ));
  }

  // 適応度の小さい順にソートする
  // ここは dijkstra の気分 (insert がないので priority_queue は不要)
  sort(ALL(all_pair));

  // 選ばれていなければ true
  vector<bool> is_solo(n, true);
  vvl all_groups;
  for (auto [_score, _rand, me, you] : all_pair)
  {
    // どちらも1人ならペアを組む
    if (is_solo[me] && is_solo[you])
    {
      all_groups.push_back(vl{ me, you });
      is_solo[me]  = false;
      is_solo[you] = false;
    }

    // 作りたいグループの数だけペアができたら終了
    if ((ll)all_groups.size() >= group_num) break;
  }
  all_pair.clear();

  vl solo;
  // 選ばれていない人を抽出する
  rep(i, n)
  {
    if (is_solo[i])
    {
      solo.push_back(i);
    }
  }
  is_solo.clear();

  // グループごとに人を追加する
  // 全員選ばれるまで
  while(!solo.empty())
  {
    for (auto& group : all_groups)
    {
      // 候補者のスコアを tuple<score, random_value, idx> で保存する
      // random_value はソート時にランダム性を持たせるため
      vector<tuple<ll, ll, ll>> candidate_score;
      rep(i, solo.size())
      {
        ll score = 0;
        for(auto me : group) score += calc_score(me, solo[i], history);
        candidate_score.push_back(make_tuple(score, rand(), i));
      }

      // スコアが一番低いメンバーを取得
      auto min_itr = min_element(ALL(candidate_score));

      // いなければ(全員選ばれていれば)終了
      if (min_itr == candidate_score.end()) break;

      // 内定者をグループに加える
      auto [_score, _random, index] = *min_itr;
      group.push_back(solo[index]);
      solo.erase(solo.begin() + index);
    }
  }
  solo.clear();

  // 続いてリーダーを選出する
  // グループの中でリーダーになった回数がいちばん少ない人を先頭にする
  for (auto& group : all_groups)
  {
    auto leader = min_element(ALL(group), [&](const auto& a, const auto& b){
      return leader_num[a] < leader_num[b];
    });
    iter_swap(leader, group.begin());
  }

  return all_groups;
}

// validation
// 差がないようにグループに振り分けられているか
// 重複なく振り分けられているか
void validate(vvl& group, ll n, ll m)
{
  // グループの個数
  ll group_num = n / m;

  const string err_group_size = "err_group_size";
  const string err_member_duplicate = "err_member_duplicate";
  ll min_v = INF;
  ll max_v = 0;
  vector<ll> bin(n, 0);
  rep(i, group_num)
  {
    chmin(min_v, (ll)group[i].size());
    chmax(max_v, (ll)group[i].size());
    for (auto v : group[i])
    {
      bin[v]++;
    }
  }
  try
  {
    if (max_v - min_v > 1)
    {
      show("max:", max_v, "min:", min_v);
      throw err_group_size;
    }
    rep(i, n) if (bin[i] > 1)
    {
      show("idx", i, "is", bin[i]);
      throw err_member_duplicate;
    }
  }
  catch (string err)
  {
    show("error occured");
    if (err == err_group_size)
    {
      show("グループの人数が不正");
    }
    if (err == err_member_duplicate)
    {
      show("メンバーが重複");
    }
    return;
  }
}

void show_group(vvl& group)
{
  cout << "[" << endl;
  for(auto vec : group)
  {
    cout << " [ ";
    for (auto v : vec) cout << v << " ";
    cout << "]," << endl;
  }
  cout << "]" << endl;
}

ll evaluate(vector<vvl>& group)
{
  ll day = group.size();
  ll n = 0; // 全メンバー数
  for (auto g : group.back()) n += g.size();
  ll group_num = group.back().size();

  // 評価関数: 日時経過によるメンバー間の重複を評価
  ll duplicate_count = 0;
  vector< vector<bool> > check(n, vector<bool>(n, false));

  rep(today, day)
  rep(i, group_num)
  {
    for (auto now : group[today][i])
    for (auto pair : group[today][i])
    {
      if (now == pair) continue;

      if (check[now][pair])
      {
        duplicate_count++;
      }
      check[now][pair] = true;
    }
  }

  show("重複数 :", duplicate_count);

  // 評価関数: リーダーの偏りを評価
  vector<ll> leader_num(n, 0);
  rep(today, day)
  rep(i, group_num)
  {
    if (group[today][i].size() == 0) continue;
    ll leader_idx = group[today][i][0];
    leader_num[leader_idx]++;
  }
  auto [min_idx, max_idx] = minmax_element(ALL(leader_num));

  show("リーダー回数の最大差 :", *max_idx - *min_idx);

  return duplicate_count + (*max_idx - *min_idx);
}

// 入力から必要なデータをつくる
void process_input(ll &n, ll &m, vector<vvl> &group_history, vvl &history, vl &leader_num)
{
  ll day;
  cin >> n >> m >> day;

  ll group_num = n / m;
  group_history.resize(day, vvl(group_num, vl()));
  history.resize(n, vl(n, 0));
  leader_num.resize(n, 0);
  rep(i, n)
  {
    rep(today, day)
    {
      string s;
      cin >> s;
      if (s != "-") // '-' を除く
      {
        ll group_idx = stoi(s) - 1;
        group_history[today][group_idx].push_back(i);
      }
    }
  }

  rep(today, day)
  {
    rep(group_idx, group_num)
    {
      for (auto  me : group_history[today][group_idx])
      for (auto you : group_history[today][group_idx])
      {
        history[me][you] += 1;
      }
    }
  }
}

void solve()
{
  // n人 を m人ずつにわける
  ll n, m;
  vector< vvl > group_history;
  // 過去に同じグループになった回数
  vvl history;
  // これまでにリーダーになった回数
  vl leader_num;

  process_input(n, m, group_history, history, leader_num);

  // グループを作る
  // group[i][j][k] = i 日目の j 番目のグループに k の人が含まれる
  vvl group = make_group(n, m, history, leader_num);

  // validation
  validate(group, n, m);

  group_history.push_back(group);

  // 答えを出力
  show_group(group);

  // 評価関数
  evaluate(group_history);
}

int main()
{
  fastio;
  solve();

  return 0;
}