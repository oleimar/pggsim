#ifndef PHENOTYPE_HPP
#define PHENOTYPE_HPP

#include <string>
#include <ostream>
#include <istream>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

//************************* Struct Phenotype ******************************

// Assumptions about GenType:
// types:
//   val_type
// member functions:
//   val_type Value()
// assumptions about val_type
//   default constructor: val_type()

// In addition to the genotypic trait values, consisting of w0, theta0 and d,
// this class also stores the the individual's real and perceived qualities q
// and p, the post-interaction values of the estimated and observed rewards
// w and R, mean action theta, actual action a, payoff, TD error delta,
// eligibility elig, and eligibility trace ztheta (i.e., these are the vales
// after the specified number of rounds of interaction during a generation).

template<typename GenType>
struct Phenotype {
// public:
    using gen_type = GenType;
    using val_type = typename gen_type::val_type;
    Phenotype(double a_w0,
        double a_theta0,
        double a_d,
        double a_q,
        double a_p,
        double a_w,
        double a_R,
        double a_theta,
        double a_a,
        double a_payoff,
        double a_delta,
        double a_elig,
        double a_ztheta,
        int a_gnum,
        int a_inum,
        bool a_female) :
        w0{a_w0},
        theta0{a_theta0},
        d{a_d},
        q{a_q},
        p{a_p},
        w{a_w},
        R{a_R},
        theta{a_theta},
        a{a_a},
        payoff{a_payoff},
        delta{a_delta},
        elig{a_elig},
        ztheta{a_ztheta},
        gnum{a_gnum},
        inum{a_inum},
        female{a_female} {}
    Phenotype(const gen_type& g = gen_type(), bool a_female = true)
        { Assign(g, a_female); }
    void Assign(const gen_type& g, bool a_female);
    void Set_q(double a_q) { q = a_q; p = q + d; }
    bool Female() const { return female; }
    static std::string ColHeads();
    // public data members
    double w0;      // value of expected reward w at start of generation
    double theta0;  // value of mean action (investment) at start of generation
    double d;      // difference (p - q) between perceived and real quality
    double q;
    double p;
    double w;
    double R;
    double theta;
    double a;
    double payoff;
    double delta;
    double elig;
    double ztheta;
    int gnum;       // group number
    int inum;       // individual number
    bool female;
};

template<typename GenType>
void Phenotype<GenType>::Assign(const gen_type& g, bool a_female)
{
    val_type val = g.Value();
    // assume val is a vector with 3 components: val[0] is w0, val[1] is
    // theta0, and val[2] is d
    w0 = val[0];
    theta0 = val[1];
    d = val[2];
    q = 1.0;
    p = q + d;
    w = w0;
    R = 0.0;
    theta = theta0;
    a = 0.0;
    payoff = 0.0;
    delta = 0.0;
    elig = 0.0;
    ztheta = 0.0;
    gnum = 0;
    inum = 0;
    female = a_female;
}

template<typename GenType>
std::string Phenotype<GenType>::ColHeads()
{
    std::string col_hds;
    col_hds += "w0";
    col_hds += "\t";
    col_hds += "theta0";
    col_hds += "\t";
    col_hds += "d";
    col_hds += "\t";
    col_hds += "q";
    col_hds += "\t";
    col_hds += "p";
    col_hds += "\t";
    col_hds += "w";
    col_hds += "\t";
    col_hds += "R";
    col_hds += "\t";
    col_hds += "theta";
    col_hds += "\t";
    col_hds += "a";
    col_hds += "\t";
    col_hds += "payoff";
    col_hds += "\t";
    col_hds += "delta";
    col_hds += "\t";
    col_hds += "elig";
    col_hds += "\t";
    col_hds += "ztheta";
    col_hds += "\t";
    col_hds += "gnum";
    col_hds += "\t";
    col_hds += "inum";
    col_hds += "\t";
    col_hds += "female";
    return col_hds;
}

//---------------------------------------------------------------------
// Output and input of Phenotype<GenType> objects

template<typename GenType>
std::ostream& operator<<(std::ostream& ostr, const Phenotype<GenType>& ph)
{
    ostr << ph.w0
        << '\t' << ph.theta0
        << '\t' << ph.d
        << '\t' << ph.q
        << '\t' << ph.p
        << '\t' << ph.w
        << '\t' << ph.R
        << '\t' << ph.theta
        << '\t' << ph.a
        << '\t' << ph.payoff
        << '\t' << ph.delta
        << '\t' << ph.elig
        << '\t' << ph.ztheta
        << '\t' << ph.gnum
        << '\t' << ph.inum
        << '\t' << ph.female;
   return ostr;
}

template<typename GenType>
std::istream& operator>>(std::istream& istr, Phenotype<GenType>& ph)
{
    istr >> ph.w0
        >> ph.theta0
        >> ph.d
        >> ph.q
        >> ph.p
        >> ph.w
        >> ph.R
        >> ph.theta
        >> ph.a
        >> ph.payoff
        >> ph.delta
        >> ph.elig
        >> ph.ztheta
        >> ph.gnum
        >> ph.inum
        >> ph.female;
    return istr;
}

#endif // PHENOTYPE_HPP
