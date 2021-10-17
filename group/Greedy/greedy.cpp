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
using ll = long long;
using vl = vector<ll>;
using vvl = vector<vector<ll>>;
using P = pair<ll, ll>;
const ll INF = 1LL << 60;
void YN(bool a) { cout << (a ? "Yes" : "No") << endl; }
template<class T> inline bool chmax(T& a, T b) { if (a < b) { a = b; return true; } return false; }
template<class T> inline bool chmin(T& a, T b) { if (a > b) { a = b; return true; } return false; }
void show(){ cout << endl; }
template <class Head, class... Tail>
void show(Head&& head, Tail&&... tail){ cout << head << " "; show(std::forward<Tail>(tail)...); }
template<class T> inline void showall(T& a) { for(auto v:a) cout<<v<<" "; cout<<endl; }

// n : 全体の人数
// m : グループの人数
// day : 作る日
// @return group[i][j][k] = i 日目の j 番目のグループに k の人が含まれる
vector<vvl> make_group(ll n, ll m, ll day)
{
  ll group_num = n / m;
  
  vvl group_base(day, vl(n));
  rep(today, day) iota(ALL(group_base[today]), 0);

  vl each_group_size(group_num, m);
  rep (i, n % group_num) each_group_size[i]++;

  // 乱数生成：無駄にメルセンヌ・ツイスタを使ってみる
  // 周期性があるのでシードがわかれば再現可能
  random_device rnd;
  int seed = rnd();
  mt19937 mt(seed);
  show("using seed :", seed);

  repi(today, 1, day)
  {
    // 前日に同じグループになった人を記録する
    vector< vector<bool> > same_group(n, vector<bool>(n, false));

    {
      ll now = 0;
      for (auto sz : each_group_size)
      {
        repi(j, now, now + sz)
        repi(k, now, now + sz)
        {
          ll ji = group_base[today - 1][j];
          ll ki = group_base[today - 1][k];
          same_group[ji][ki] = same_group[ki][ji] = true;
        }
        now += sz;
      }
    }

    {
      // Fisher–Yates shuffle で que につめる
      queue<ll> que;
      queue<ll> wainting;
      vector<ll> pool(n);
      iota(ALL(pool), 0);
      rep(i, n)
      {
        ll idx = mt() % (n - i);
        que.push(pool[idx]);
        swap(pool[idx], pool[n - i - 1]);
      }

      ll now = 0;
      ll now_group = 0;
      vector<ll> valid;
      while (!wainting.empty() || !que.empty())
      {
        bool force_flag = false;
        if (que.empty())
        {
          while(!wainting.empty()) que.push(wainting.front()), wainting.pop();
          force_flag = true;
        }

        ll next = que.front();
        que.pop();

        if (!force_flag)
        {
          ll ng = false;
          for (auto v : valid) ng |= same_group[next][v];

          if (ng) // 既存メンバーと重複した
          {
            wainting.push(next);
            continue;
          }
        }

        // 既存メンバーと重複しない

        // 同じグループになったフラグを更新
        for (auto v : valid) same_group[v][next] = same_group[next][v] = true;
        // グループに追加
        valid.push_back(next);

        // 規定の人数を超えたら
        if ((ll)valid.size() >= each_group_size[now_group])
        {
          rep(i, each_group_size[now_group])
          {
            group_base[today][now + i] = valid[i];
          }
          now += each_group_size[now_group];
          now_group++;
          valid.resize(0);
        }
      }
    }
  }
  
  vector<vvl> ret(day, vvl(group_num, vl(0)));
  rep(today, day)
  {
    ll now = 0;
    rep (i, group_num)
    {
      repi(idx, now, now + each_group_size[i])
      {
        ret[today][i].push_back(group_base[today][idx]);
      }
      now += each_group_size[i];
      // showall(ret[today][i]);
    }
  }
  
  return ret;
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
  ll n, m;
  ll day = 2;
  cin >> n >> m;

  // グループを作る
  // group[i][j][k] = i 日目の j 番目のグループに k の人が含まれる
  vector< vvl > group = make_group(n, m, day);

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