#ifndef ACGROUP_HPP
#define ACGROUP_HPP

#include <vector>
#include <random>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

//************************ Class ActCritGroup *****************************

// This class sets up and simulates the actor-critic learning method for a
// public goods investment game.

// The code uses eligibility traces for learning updates of the preference.
// However, because there is only one state, it is meaningless to use
// eligibility for the values (cf. Box in section 13.6 of Sutton and Barto).

// The class deals with the interactions in one group, over the time steps
// during one generation.

// The following is assumed about the template parameter PhenType
// 1. It is assignable
// 2. It has the following public members of type double:
//    q, p, w, R, theta, a, payoff, delta, elig, ztheta

template<typename PhenType>
class ActCritGroup {
public:
    using phen_type = PhenType;
    using v_type = std::vector<phen_type>;
    using rand_eng = std::mt19937;
    using rand_uni = std::uniform_real_distribution<double>;
    using rand_int = std::uniform_int_distribution<int>;
    using rand_norm = std::normal_distribution<double>;
    ActCritGroup(int a_g,
                 int a_T,
                 double a_B0,
                 double a_B1,
                 double a_B2,
                 double a_K1,
                 double a_K11,
                 double a_K12,
                 double a_sigma,
                 double a_alphaw,
                 double a_alphatheta,
                 double a_lambdatheta,
                 const v_type& a_memb);
    const v_type& Get_memb() const { return memb; }
    void Interact(rand_eng& eng);

private:
    void Update_R_payoff();
    int g;              // group size
    int T;              // number of rounds for group interaction
    double B0;          // payoff parameter
    double B1;          // payoff parameter
    double B2;          // payoff parameter
    double K1;          // payoff parameter
    double K11;         // payoff parameter
    double K12;         // payoff parameter
    double sigma;       // SD of action distribution
    double alphaw;      // learning rate
    double alphatheta;  // learning rate
    double lambdatheta; // eligibility trace parameter
    v_type memb;        // members of the group
};

template<typename PhenType>
ActCritGroup<PhenType>::ActCritGroup(int a_g,
    int a_T,
    double a_B0,
    double a_B1,
    double a_B2,
    double a_K1,
    double a_K11,
    double a_K12,
    double a_sigma,
    double a_alphaw,
    double a_alphatheta,
    double a_lambdatheta,
    const v_type& a_memb) :
    g{a_g},
    T{a_T},
    B0{a_B0},
    B1{a_B1},
    B2{a_B2},
    K1{a_K1},
    K11{a_K11},
    K12{a_K12},
    sigma{a_sigma},
    alphaw{a_alphaw},
    alphatheta{a_alphatheta},
    lambdatheta{a_lambdatheta},
    memb{a_memb}
{
}

template<typename PhenType>
void ActCritGroup<PhenType>::Interact(rand_eng& eng)
{
    rand_norm nrm(0.0, 1.0);
    // set payoff values to zero at start of generation
    for (auto& m : memb) {
        m.payoff = 0.0;
    }
    // run through the time steps
    for (int step = 0; step < T; ++step) {
        // set actions for group members
        for (auto& m : memb) {
            m.a = m.theta + sigma*nrm(eng);
        }
        // assign rewards and payoff increments
        Update_R_payoff();
        // update actor-critic learning parameters
        for (auto& m : memb) {
            // TD error
            m.delta = m.R - m.w;
            // NOTE: code to avoid too large values of the TD error
            double deltalim = 0.5;
            if (m.delta > deltalim) {
                m.delta = deltalim;
            } else if (m.delta < -deltalim) {
                m.delta = -deltalim;
            }
            // update w
            m.w += alphaw*m.delta;
            m.elig = (m.a - m.theta)/(sigma*sigma);
            m.ztheta = lambdatheta*m.ztheta + m.elig;
            // NOTE: code to avoid too large values of the eligibility trace
            double eltracelim = 5.0/sigma;
            if (m.ztheta > eltracelim) {
                m.ztheta = eltracelim;
            } else if (m.ztheta < -eltracelim) {
                m.ztheta = -eltracelim;
            }
            // update theta
            m.theta += alphatheta*m.ztheta*m.delta;
        }
    }
    // scale payoff to be per time step
    if (T > 0) {
        for (auto& m : memb) {
            m.payoff /= T;
        }
    }
}

template<typename PhenType>
void ActCritGroup<PhenType>::Update_R_payoff()
{
    // assign rewards and accumulate payoffs
    double av_a = 0.0;
    for (const auto& m : memb) {
        av_a += m.a;
    }
    av_a /= g;
    double B = B0 + B1*av_a + 0.5*B2*av_a*av_a;
    for (auto& m : memb) {
        m.R = B - (K1 + 0.5*K11*m.a + K12*m.p)*m.a;
        m.payoff += B - (K1 + 0.5*K11*m.a + K12*m.q)*m.a;
    }
}

#endif // ACGROUP_HPP
