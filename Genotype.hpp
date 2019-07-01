#ifndef GENOTYPE_HPP
#define GENOTYPE_HPP

#include <random>
#include <cmath>
#include <array>
#include <string>
#include <ostream>
#include <istream>
#include <sstream>


// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

//*********************** Struct MutIncrUni **********************************

// The purpose of this struct is to model real-valued, standardized mutational
// increments with a uniform (rectangular) distribution. Standardized means
// that the increment has mean zero and standard deviation one.

template<typename RandEng = std::mt19937>
struct MutIncrUni {
// public:
    using rand_eng = RandEng;
    using rand_uni = std::uniform_real_distribution<double>;
    rand_uni uni{-std::sqrt(3.0), std::sqrt(3.0)};
    // from minus to plus square root of 3 to get variance one
    double StdIncr(rand_eng& eng) { return uni(eng); }
};


//*********************** Struct MutIncrNorm *********************************

// Same as previous but with normally distributed increments

template<typename RandEng = std::mt19937>
struct MutIncrNorm {
// public:
    using rand_eng = RandEng;
    using rand_norm = std::normal_distribution<double>;
    rand_norm nrm{0.0, 1.0};
    double StdIncr(rand_eng& eng) { return nrm(eng); }
};


//*********************** Struct MutIncrBiExp ********************************

// Same as previous but with Laplacian distributed increments

template<typename RandEng = std::mt19937>
struct MutIncrBiExp {
// public:
    using rand_eng = RandEng;
    using rand_bool = std::bernoulli_distribution;
    using rand_exp = std::exponential_distribution<double>;
    rand_bool bl{0.5};
    // parameter lambda = square root of 2 to get variance one
    rand_exp ex{std::sqrt(2.0)};
    double StdIncr(rand_eng& eng) { return bl(eng) ? ex(eng) : -ex(eng); }
};


//*********************** Struct MutRec **************************************

// The purpose of this struct is to serve as a wrapper for parameters for
// mutation rates, a struct to define a mutational distribution, upper and
// lower limits of allelic effects, and parameters for recombination rates. The
// template parameter n_loci is the number of loci (each locus codes for a
// one-dimensional trait). Note that the pseudo-random engine is the same for
// this struct as for the mutational increment struct

template<std::size_t n_loci, typename MutIncr>
struct MutRec {
// public:
    using mut_incr_type = MutIncr;
    using rand_eng = typename mut_incr_type::rand_eng;
    using rand_uni = std::uniform_real_distribution<double>;
    // void SetRandEng(rand_eng& engine) { eng = engine; }
    MutRec(rand_eng& a_eng) : eng{a_eng} {}
    void SetMutRate(double m) { mut_rate.fill(m); }
    void SetMutRate(const std::array<double, n_loci>& m) { mut_rate = m; }
    void SetSD(const std::array<double, n_loci>& sd) { SD = sd; }
    void SetMaxVal(const std::array<double, n_loci>& m) { max_val = m; }
    void SetMinVal(const std::array<double, n_loci>& m) { min_val = m; }
    void SetRho(const std::array<double, n_loci>& r) { rho = r; }
    void SetRho(double r) { rho.fill(r); }
    double StdIncr() { return mi.StdIncr(eng); }
    rand_eng& eng;                          // random number generator
    rand_uni uni{0.0, 1.0};                 // uniform on unit interval
    mut_incr_type mi;                       // mutational increment object
    std::array<double, n_loci> mut_rate;    // probability of mutation
    std::array<double, n_loci> SD;          // SD of mutational increments
    std::array<double, n_loci> max_val;     // upper bounds for trait
    std::array<double, n_loci> min_val;     // lower bounds for trait
    std::array<double, n_loci> rho;         // recombination rate
};


//************************* Struct Gamete ************************************

// For this gamete implementation, each of n_loci loci code for a separate
// one-dimensional trait. A number of 'parameters', relating to mutation, are
// present as members of the template parameter type MutRec, including a member
// StdIncr() that returns a real-valued, standardized (mean zero, variance one)
// mutational increment.

template<std::size_t n_loci, typename MutRec>
struct Gamete {
// public:
    using mut_rec_type = MutRec;
    using gam_data_type = std::array<double, n_loci>;
    using val_type = gam_data_type;
    using rho_vec_type = std::array<double, n_loci>;
    static const std::size_t num_loci = n_loci;
    static const std::size_t num_traits = n_loci;
    Gamete(gam_data_type&& g = gam_data_type()) : gamdat{g} {}
    Gamete(const gam_data_type& g) : gamdat{g} {}
    const val_type& Value() const { return gamdat; }
    double& operator[](std::size_t i) { return gamdat[i]; }
    double operator[](std::size_t i) const { return gamdat[i]; }
    std::size_t size() const { return gamdat.size(); }
    void Mutate(MutRec& mr);
    // public data member
    gam_data_type gamdat;
};

template<std::size_t n_loci, typename MutRec>
void  Gamete<n_loci, MutRec>::Mutate(MutRec& mr)
{
   for (std::size_t i = 0; i < gamdat.size(); ++i) {
      // mutate allele at locus i
      if ( mr.mut_rate[i] > 0 ) {
         if ( mr.uni(mr.eng) < mr.mut_rate[i] ) {
            double u = mr.StdIncr();
            // u has mean zero and variance one, which means that
            // SD[i]*SD[i] will be the variance of mutational increments
            gamdat[i] += mr.SD[i]*u;
            if ( gamdat[i] > mr.max_val[i] ) gamdat[i] = mr.max_val[i];
            else if ( gamdat[i] < mr.min_val[i] ) gamdat[i] = mr.min_val[i];
         }
      }
   }
}


//---------------------------------------------------------------------
// Output and input of Gamete objects

template<std::size_t n_loci, typename MutRec>
std::ostream& operator<<(std::ostream& ostr,
                         const Gamete<n_loci, MutRec>& g)
{
    for (std::size_t i = 0; i < g.size(); ++i) {
        ostr << g[i];
        if (i < g.size() - 1) ostr << '\t';
    }
    return ostr;
}

template<std::size_t n_loci, typename MutRec>
std::istream& operator>>(std::istream& istr,
                         Gamete<n_loci, MutRec>& g)
{
    for (std::size_t i = 0; i < g.size(); ++i) {
        istr >> g[i];
    }
    return istr;
}


//*********************** Struct Diplotype ***********************************

// This struct represents a diploid genotype consisting of a maternal and a
// paternal gamete. The member function GetGamete forms a new gamete of type
// GamType from the genotype, allowing for Mendelian segregation,
// recombination, and mutation. A number of parameters relating to mutation,
// segregation and recombination are present as members of the template
// parameter type MutRec. The array rho in a MutRec objects controls
// segregation and recombination: rho[0] is the probability that the allele
// from locus 0 will come from the maternal gamete (should be set to 0.5 for
// Mendelian segregation) and rho[i] is the probability of recombination
// between locus i-1 and i.

// The following is assumed about the template parameter GamType
// 1. It is assignable and has default and copy constructors
// 2. It has the types mut_rec_type, gam_data_type and val_type
// 3. It has the static members num_loci and num_traits
// 4. It has a member function Value() which returns a val_type,
//    or a const val_type&
// 5. It has a member function Mutate(mut_rec_type& mr)
// 6. It has operators << and >> for output and input

template<typename GamType>
struct Diplotype {
// public:
    using gam_type = GamType;
    using mut_rec_type = typename gam_type::mut_rec_type;
    using gam_data_type = typename gam_type::gam_data_type;
    using val_type = typename gam_type::val_type;
    using rho_vec_type = typename gam_type::rho_vec_type;
    static const std::size_t num_loci = gam_type::num_loci;
    static const std::size_t num_traits = gam_type::num_traits;
    Diplotype(gam_type&& g = gam_type()) :
        mat_gam{g}, pat_gam{g} {}
    Diplotype(const gam_type& g) :
        mat_gam{g}, pat_gam{g} {}
    Diplotype(gam_type&& mg, gam_type&& pg) :
        mat_gam{mg},
        pat_gam{pg} {}
    void Assign(gam_type&& mg, gam_type&& pg)
    { mat_gam = mg; pat_gam = pg; }
    gam_type& MatGam() { return mat_gam; }
    const gam_type& MatGam() const { return mat_gam; }
    gam_type& PatGam() { return pat_gam; }
    const gam_type& PatGam() const { return pat_gam; }
    gam_type GetGamete(mut_rec_type& mr) const;
    gam_type GetGamete(mut_rec_type& mr,
                      const rho_vec_type& rhov) const;
    val_type Value() const;
    val_type MatVal() const { return mat_gam.Value(); }
    val_type PatVal() const { return pat_gam.Value(); }
    static std::string ColHeads();
    // public data members
    gam_type mat_gam;
    gam_type pat_gam;
};

template<typename GamType>
typename Diplotype<GamType>::gam_type
Diplotype<GamType>::GetGamete(mut_rec_type& mr) const
{
    gam_type gam;
    gam_data_type& gam_data = gam.gamdat;
    const gam_data_type& mat_gam_data = mat_gam.gamdat;
    const gam_data_type& pat_gam_data = pat_gam.gamdat;
    // random segregation (Mendelian when rho[0] is 0.5)
    bool mat = (mr.uni(mr.eng) < mr.rho[0]);
    gam_data[0] = mat ? mat_gam_data[0] : pat_gam_data[0];
    for (std::size_t i = 1; i < gam_data.size(); ++i) {
        // recombination between locus i-1 and locus i
        if (mr.uni(mr.eng) < mr.rho[i]) mat = !mat;
        gam_data[i] = mat ? mat_gam_data[i] : pat_gam_data[i];
    }
    // mutation
    gam.Mutate(mr);
    return gam;
}

// This method can be used when recombination rates can change (e.g.,
// evolution); the passed rhov contains the recombination rates
template<typename GamType>
typename Diplotype<GamType>::gam_type
Diplotype<GamType>::GetGamete(mut_rec_type& mr,
                                   const rho_vec_type& rhov) const
{
    gam_type gam;
    gam_data_type& gam_data = gam.gamdat;
    const gam_data_type& mat_gam_data = mat_gam.gamdat;
    const gam_data_type& pat_gam_data = pat_gam.gamdat;

    // random segregation (Mendelian when rho[0] is 0.5)
    bool mat = (mr.uni(mr.eng) < rhov[0]);
    gam_data[0] = mat ? mat_gam_data[0] : pat_gam_data[0];
    for (std::size_t i = 1; i < gam_data.size(); ++i) {
        // recombination between locus i-1 and locus i
        if (mr.uni(mr.eng) < rhov[i]) mat = !mat;
        gam_data[i] = mat ? mat_gam_data[i] : pat_gam_data[i];
    }
    // mutation
    gam.Mutate(mr);
    return gam;
}

// The value of a diploid genotype is the sum of the values of the maternal
// and paternal gametes.
template<typename GamType>
typename Diplotype<GamType>::val_type
Diplotype<GamType>::Value() const
{
    val_type v = mat_gam.Value();
    const val_type& vp = pat_gam.Value();
    for (std::size_t i = 0; i < v.size(); ++i) {
        v[i] += vp[i];
    }
    return v;
}

template<typename GamType>
std::string Diplotype<GamType>::ColHeads()
{
    std::string col_hds;
    for (std::size_t loc = 0; loc < num_loci; ++loc) {
        std::ostringstream osl;
        osl << (loc + 1);
        col_hds += "Mat";
        col_hds += osl.str();
        col_hds += "\t";
    }
    for (std::size_t loc = 0; loc < num_loci; ++loc) {
        std::ostringstream osl;
        osl << (loc + 1);
        col_hds += "Pat";
        col_hds += osl.str();
        col_hds += "\t";
    }
    // remove final tab character
    std::size_t i = col_hds.length()-1;
    col_hds.erase(i,1);
    return col_hds;
}

//---------------------------------------------------------------------
// Output and input of Diplotype objects

template<typename GamType>
std::ostream& operator<<(std::ostream& ostr,
                         const Diplotype<GamType>& g)
{
    ostr << g.MatGam() << '\t' << g.PatGam();
    return ostr;
}

template<typename GamType>
std::istream& operator>>(std::istream& istr,
                         Diplotype<GamType>& g)
{
    istr >> g.MatGam() >> g.PatGam();
    return istr;
}


//*********************** Struct Haplotype ***********************************

// This struct represents a haploid genotype consisting of a gamete. The member
// function GetGamete forms a new gamete of type GamType from the genotype,
// allowing for mutation.

// The following is assumed about the template parameter GamType
// 1. It is assignable and has default and copy constructors
// 2. It has the types mut_rec_type and val_type
// 3. It has the static members num_loci and num_traits
// 4. It has a member function Value() which returns a val_type,
//    or a const val_type&
// 5. It has a member function Mutate(mut_rec_type& mr)
// 6. It has operators << and >> for output and input

template<typename GamType>
struct Haplotype {
// public:
    using gam_type = GamType;
    using mut_rec_type = typename gam_type::mut_rec_type;
    using val_type = typename gam_type::val_type;
    static const std::size_t num_loci = gam_type::num_loci;
    static const std::size_t num_traits = gam_type::num_traits;
    Haplotype(gam_type&& g = gam_type()) : gam{g} {}
    void Assign(gam_type&& g) { gam = g; }
    gam_type& Gam() { return gam; }
    const gam_type& Gam() const { return gam; }
    gam_type GetGamete(mut_rec_type& mr) const;
    val_type Value() const { return gam.Value(); }
    static std::string ColHeads();
    // public data member
    gam_type gam;
};

template<typename GamType>
typename Haplotype<GamType>::gam_type
Haplotype<GamType>::GetGamete(mut_rec_type& mr) const
{
    gam_type new_gam = gam;
    new_gam.Mutate(mr);
    return new_gam;
}

template<typename GamType>
std::string Haplotype<GamType>::ColHeads()
{
    std::string col_hds;
    for (std::size_t loc = 0; loc < num_loci; ++loc) {
        std::ostringstream osl;
        osl << (loc + 1);
        col_hds += "Loc";
        col_hds += osl.str();
        col_hds += "\t";
    }
    // remove final tab character
    std::size_t i = col_hds.length()-1;
    col_hds.erase(i,1);
    return col_hds;
}

//---------------------------------------------------------------------
// Output and input of Haplotype objects

template<typename GamType>
std::ostream& operator<<(std::ostream& ostr,
                         const Haplotype<GamType>& g)
{
    ostr << g.Gam();
    return ostr;
}

template<typename GamType>
std::istream& operator>>(std::istream& istr,
                         Haplotype<GamType>& g)
{
    istr >> g.Gam();
    return istr;
}


#endif // GENOTYPE_HPP
