#include <vector>
#include <iostream>
#include <fstream>

class Gain {
private:
    std::vector<std::vector<int>> gain;
    std::vector<int> strategy_;
    size_t n;
    int compos;
public:
    Gain(std::vector<int> strategy) : strategy_(strategy), n(strategy.size()) {
        compos = 1;
        for (size_t i = 0; i != n; ++i) {
            if (strategy[i] == 0) {
                continue;
            }
            compos *= strategy[i];
        }
        gain.resize(compos, std::vector<int>(n));
    }
    const std::vector<int>& operator() (size_t j) const {
        std::vector<int> temp(n);
        for (size_t i = 0; i != n; ++i) {
            temp[i] = gain[j][i];
        }
        return gain[j];
    }

    std::vector<int>& operator() (size_t j) {
        std::vector<int> temp(n);
        for (size_t i = 0; i != n; ++i) {
            temp[i] = gain[j][i];
        }
        return gain[j];
    }

    const std::vector<int>& operator() (std::vector<int> profile) const {
        int numb = 0;
        int k = 1;
        for (size_t i = n-1; i >= 0; --i) {
            numb += profile[i] * k;
            k *= strategy_[i];
        }
        return gain[numb];
    }

    std::vector<int>& operator() (std::vector<int> profile)  {
        int numb = 0;
        int k = 1;
        for (size_t i = n-1; i >= 0; --i) {
            numb += profile[i] * k;
            k *= strategy_[i];
        }
        return gain[numb];
    }

    size_t Size() {
        return gain.size();
    }

    std::vector<int> Dom_strat(int player) {
        int step = 1;
        for (int i = player + 1; i < strategy_.size(); ++i)
            step *= strategy_[i];
        int i = 0;
        std::vector<bool> bestStrats(strategy_[player], 1);
        while (i < this->Size()) {
            for (int k = 0; k < step; ++k) {
                int bestStatInProfile = 0;
                for (int j = 1; j < strategy_[player]; ++j) {// i + k + (j * step)
                    if (gain[i + k + (j * step)][player] > gain[i + k + (bestStatInProfile * step)][player]) {
                        bestStrats[bestStatInProfile] = 0;
                        bestStatInProfile = j;
                    } else if (gain[i + k + (j * step)][player] < gain[i + k + (bestStatInProfile * step)][player]){
                        bestStrats[j] = 0;
                    }
                }
            }
            i += step * strategy_[player];
        }
        std::vector<int> ans;
        for (int j = 0; j < bestStrats.size(); ++j)
            if (bestStrats[j])
                ans.push_back(j);
        return ans;
    }

    std::vector<int> Nash_equil() {
        std::vector<int> ans;
        for (int i = 0; i <gain.size(); ++i) {
            std::vector<int> profile(n);
            profile = profToNumb(i);
            int p = 1;
            bool NashEq = 1;
            for (int j = int(n) - 1; j >= 0; --j) {
                for (int k = 0; k < strategy_[j]; ++k) {
                    if (profile[j] == k)
                        continue;
                    if (gain[i][j] < gain[i - ((profile[j] - k) * p)][j])
                        NashEq = 0;
                }
                p *= strategy_[j];
            }
            if (NashEq)
                ans.push_back(i);
        }
        return ans;
    }

    std::vector<int> profToNumb(int i) {
        int temp = compos;
        std::vector<int> ans(n);
        int k = i;
        for (int j = 0; j < n; ++j) {
            temp /= strategy_[j];
            ans[j] = i / temp;
            i %= temp;
        }
        return ans;
    }
};

int main() {
    std::ifstream d("input.txt");
    int n, stroke;
    d >> n;
    std::vector<int> strategy(n);
    for (size_t i = 0; i != n; ++i)
        d >> strategy[i];
    Gain gain(strategy);
    for (size_t i = 0; i < gain.Size(); ++i)
        for (size_t j = 0; j < n; ++j)
            d >> gain(i)[j];
    std::vector<int> max(n);
    int paretto_dom = -1;
    for (size_t i = 0; i < gain.Size(); ++i) {   // paretto
        bool flag = 0;
        bool brokeAns = 0;
        for (size_t j = 0; j < n; ++j) {
            if (gain(i)[j] > max[j]) {
                max[j] = gain(i)[j];
                brokeAns = 1;
            } else {
                flag = 1;
            }
        }
        if (!flag) {
            paretto_dom = i;
        } else {
            if (brokeAns)
                paretto_dom = -1;
        }
    }
    if (paretto_dom == -1)
        std::cout << "There is no Pareto optimal profile in this game";
    else
        std::cout << "Pareto optimal profile is: " << paretto_dom;
    std::cout << "\nNash equilibrium profiles:\n";
    std::vector<int> nash;
    nash = gain.Nash_equil();
    for (size_t i = 0; i < nash.size(); ++i) {
        std::cout << nash[i] << ' ';
    }
    int dom_strat_numbers;
    d >> dom_strat_numbers;
    std::vector<int> dom_strats;
    for (int i = 0; i < dom_strat_numbers; ++i) {
        int temp;
        d >> temp;
        std::cout << "\nFor player " << temp << " dominant strategies:\n";
        dom_strats = gain.Dom_strat(temp - 1);
        if (dom_strats.size() == 0)
            std::cout << "There is no dominant strat for player " << temp;
        else {
            std::cout << "Dominant strategies for player " << temp << ":\n";
            for (size_t j = 0; j < dom_strats.size(); ++j)
                std::cout << dom_strats[i] << ' ';
        }
    }
}