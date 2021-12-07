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

class UnionFind{
public:
  vector<ll> p;		// 親
  vector<ll> rank;	// サイズ・各集合の根のみ有効
  ll root_num; // 連結成分の数
  UnionFind(ll n) : root_num(n) {
    p.resize(n, -1);
    rank.resize(n, 1);
  }
  ll root(ll x){
    if(p[x] == -1) return x;
    else return p[x] = root(p[x]); // 深さを 1 にしている
  }
  bool unite(ll x, ll y){
    x = root(x); y = root(y);
    if(x == y) return false;
    if(rank[x] > rank[y]) swap(x, y); // rankの小さいものを下につける
    rank[y] += rank[x];
    p[x] = y;
    root_num--;
    return true;
  }
  // グループごとに頂点をまとめる: O(N log N)
  map<ll, vector<ll>> groups(){
    map<ll, vector<ll>> ret;
    rep(i, p.size()) ret[root(i)].emplace_back(i);
    return ret;
  }
  //xが属すグループのサイズ
  ll size(ll x){ return rank[root(x)]; }
  bool same(ll x, ll y){ return (root(x) == root(y)); }
};

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
vector<vvl> make_group(ll n, ll m, ll day, const vvl &history, const vl &leader_num)
{
  // できるグループの数
  ll group_num = n / m;

  // pair<score, me, you>
  vector<tuple<ll, ll, ll>> all_pair;
  rep(i1, n)
  rep(i2, i1)
  {
    all_pair.push_back(make_tuple(
        calc_score(i1, i2, history),
        i1,
        i2
      ));
  }

  // 適応度の小さい順にソートする
  sort(ALL(all_pair));

  // グループ情報を UF で管理する
  UnionFind uf(n);
  for (auto [_, me, you] : all_pair)
  {
    // どちらも1人ならペアを組む
    if (uf.size(me) == 1 && uf.size(you) == 1)
    {
      uf.unite(me, you);
    }

    // 作りたいグループの数だけペアができたら終了
    if (uf.root_num <= group_num) break;
  }

  // 選ばれていなければ true
  vector<bool> is_solo(n, false);
  vvl ret(group_num);
  ll group_idx = 0;

  // できたグループごとに
  for (auto mp : uf.groups())
  {
    auto p = mp.second;
    // ひとり
    if (p.size() == 1) is_solo[p[0]] = true;
    // ペア
    else if (p.size() == 2)
    {
      ret[group_idx].push_back(p[0]);
      ret[group_idx].push_back(p[1]);
      group_idx++;
    }
    // それ以外はない
    else assert(false && "uf.groups().size() is not 1 or 2");
  }

  // グループごとに人を追加する
  // 選ばれていない人がいなくなるまで
  while(count(ALL(is_solo), true) > 0)
  {
    for (auto& group : ret)
    {
      // 候補者のスコアを pair<score, idx> で保存する
      vp candidate_score;
      rep(candidate, n)
      {
        if (is_solo[candidate])
        {
          ll score = 0;
          for(auto me : group) score += calc_score(me, candidate, history);
          candidate_score.push_back(make_pair(score, candidate));
        }
      }
      // 一番スコアの低い人をグループに加える
      ll fixed = min_element(ALL(candidate_score))->second;
      group.push_back(fixed);
      is_solo[fixed] = false;
      uf.unite(group[0], fixed);
    }
  }

  // 続いてリーダーを選出する
  // グループの中でリーダーになった回数がいちばん少ない人を先頭にする
  for (auto& group : ret)
  {
    auto leader = min_element(ALL(group), [&](const auto& a, const auto& b){
      return leader_num[a] < leader_num[b];
    });
    iter_swap(leader, group.begin());
  }

  vector<vvl> tmp_ret(day, vvl(group_num, vl(0)));  
  return tmp_ret;
}

// validation
// 差がないようにグループに振り分けられているか
// 重複なく振り分けられているか
void validate(vector<vvl>& group, ll n, ll m, ll day)
{
  // グループの個数
  ll group_num = n / m;

  const string err_group_size = "err_group_size";
  const string err_member_duplicate = "err_member_duplicate";
  rep(today, day)
  {
    ll min_v = INF;
    ll max_v = 0;
    vector<ll> bin(n, 0);
    rep(i, group_num)
    {
      chmin(min_v, (ll)group[today][i].size());
      chmax(max_v, (ll)group[today][i].size());
      for (auto v : group[today][i])
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
}

void show_group(vector<vvl>& group, ll day)
{
  rep(today, day)
  {
    cout << today << " : [";
    for(auto vec : group[today])
    {
      cout << " [ ";
      for (auto v : vec) cout << v << " ";
      cout << "],";
    }
    cout << " ]" << endl;
  }
}

ll evaluate(vector<vvl>& group, ll day)
{
  ll n = 0; // 全メンバー数
  for (auto g : group[0]) n += g.size();
  ll group_num = group[0].size();

  // 評価関数: 日時経過によるメンバー間の重複を評価
  ll duplicate_count = 0;
  vector<ll> check(n, 0LL);
  rep(i, n) check[i] = (1LL << i);

  rep(today, day)
  rep(i, group_num)
  {
    for (auto now : group[today][i])
    for (auto pair : group[today][i])
    {
      if (now == pair) continue;

      if (check[now] >> pair & 1)
      {
        duplicate_count++;
      }
      check[now] |= (1LL << pair);
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

void solve()
{
  // n人 を m人ずつにわける
  ll n, m, day;
  cin >> n >> m >> day;

  // 過去に同じグループになった回数
  vvl history(n, vl(n, 0));
  // これまでにリーダーになった回数
  vl leader_num(n, 0);

  // グループを作る
  // group[i][j][k] = i 日目の j 番目のグループに k の人が含まれる
  vector< vvl > group = make_group(n, m, day, history, leader_num);

  // validation
  validate(group, n, m, day);

  // 答えを出力
  show_group(group, day);

  // 評価関数
  evaluate(group, day);
}

int main()
{
  fastio;
  solve();

  return 0;
}