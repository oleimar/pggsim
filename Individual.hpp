#ifndef INDIVIDUAL_HPP
#define INDIVIDUAL_HPP

#include <utility>
#include <string>
#include <ostream>
#include <istream>
#include <sstream>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

//************************ Struct Individual *****************************

// This struct represents an individual with genotype of type GenType,
// phenotype of type PhenType, and subpopulation number, spn, of type
// std::size_t


// Assumptions about GenType:
// types:
//    gam_type
//    mut_rec_type
// constructors:
//    GenType()
//    GenType(gam_type&& gam)
//    GenType(gam_type&& mat_gam, gam_type&& pat_gam)
// member functions:
//    void Assign(gam_type&& gam)
//    void Assign(gam_type&& mat_gam, gam_type&& pat_gam)
//    gam_type GetGamete(mut_rec_type& mr)
// static member function:
//    std::string ColHeads();

// Assumptions about PhenType
// constructors:
//    PhenType()
//    PhenType(const GenType& genotype)
// member functions:
//    void Assign(const GenType& genotype)
//    bool Female()
// static member functions
//    std::string ColHeads();


template<typename GenType, typename PhenType>
struct Individual {
// public:
    using gen_type = GenType;
    using gam_type = typename gen_type::gam_type;
    using mut_rec_type = typename gen_type::mut_rec_type;
    using rho_vec_type = typename gen_type::rho_vec_type;
    using phen_type = PhenType;
    Individual() : alive(false) {}
    Individual(gen_type&& g, phen_type&& ph, std::size_t a_spn, bool a_alive) :
        genotype{g},
        phenotype{ph},
        spn{a_spn},
        alive{a_alive} {}
    // Construct individual from one gamete and a spn
    Individual(gam_type&& gam, std::size_t a_spn) :
        genotype(std::forward<gam_type>(gam)),
        phenotype(genotype),
        spn{a_spn},
        alive{true} {}
    Individual(const gam_type& gam, std::size_t a_spn) :
        genotype(gam),
        phenotype(genotype),
        spn{a_spn},
        alive{true} {}
    // Construct individual from maternal and paternal gametes and a spn
    Individual(gam_type&& mat_gam, gam_type&& pat_gam, std::size_t a_spn) :
        genotype(std::forward<gam_type>(mat_gam),
                 std::forward<gam_type>(pat_gam)),
        phenotype(genotype),
        spn{a_spn},
        alive{true} {}
    void Assign(gam_type&& gam, std::size_t a_spn);
    void Assign(gam_type&& mat_gam, gam_type&& pat_gam, std::size_t a_spn);
    void Assign(gen_type&& g, phen_type&& ph, std::size_t a_spn, bool a_alive);
    gam_type GetGamete(mut_rec_type& mr) const
    { return genotype.GetGamete(mr); }
    gam_type GetGamete(mut_rec_type& mr, const rho_vec_type& rho) const
    { return genotype.GetGamete(mr, rho); }
    const gen_type& Genotype() const { return genotype; }
    gen_type& Genotype() { return genotype; }
    const phen_type& Phenotype() const { return phenotype; }
    phen_type& Phenotype() { return phenotype; }
    std::size_t SubPopNum() const { return spn; }
    void SetSubPopN(std::size_t a_spn) { spn = a_spn; }
    bool Alive() const { return alive; }
    void SetAlive() { alive = true; }
    void SetDead() { alive = false; }
    bool Female() const { return phenotype.Female(); }
    void SetFemale(bool female) { phenotype.female = female; }
    static std::string ColHeads();
    // public data members
    gen_type genotype;
    phen_type phenotype;
    std::size_t spn;
    bool alive;           // whether individual is present in a population
};

// Construct individual from one gamete and a spn
template<typename GenType, typename PhenType>
void Individual<GenType, PhenType>::Assign(gam_type&& gam, std::size_t a_spn)
{
    genotype.Assign(gam);
    phenotype.Assign(genotype);
    spn = a_spn;
    alive = true;
}

// Construct individual by assigning maternal and paternal gametes and a spn
template<typename GenType, typename PhenType>
void Individual<GenType, PhenType>::Assign(gam_type&& mat_gam,
    gam_type&& pat_gam, std::size_t a_spn)
{
    genotype.Assign(mat_gam, pat_gam);
    phenotype.Assign(genotype);
    spn = a_spn;
    alive = true;
}

// Construct an individual by assigning all its data
template<typename GenType, typename PhenType>
void Individual<GenType, PhenType>::Assign(gen_type&& g, phen_type&& ph,
    std::size_t a_spn, bool a_alive)
{
    genotype = g;
    phenotype = ph;
    spn = a_spn;
    alive = a_alive;
}


template<typename GenType, typename PhenType>
std::string Individual<GenType, PhenType>::ColHeads()
{
    std::string col_hds = gen_type::ColHeads();
    col_hds += "\t";
    col_hds += phen_type::ColHeads();
    col_hds += "\t";
    col_hds += "SubPop";
    col_hds += "\t";
    col_hds += "Alive";
    return col_hds;
}


//---------------------------------------------------------------------
// Ouput and input of Individual<T> objects

template<typename GenType, typename PhenType>
std::ostream& operator<<(std::ostream& ostr,
                         const Individual<GenType, PhenType>& ind)
{
    ostr << ind.Genotype() << '\t'
         << ind.Phenotype() << '\t'
         << ind.SubPopNum() << '\t'
         << ind.Alive();
    return ostr;
}

template<typename GenType, typename PhenType>
std::istream& operator>>(std::istream& istr,
                         Individual<GenType, PhenType>& ind)
{
    GenType g;
    PhenType p;
    std::size_t n;
    bool a;
    istr >> g
         >> p
         >> n
         >> a;
    ind.Assign(std::move(g), std::move(p), n, a);
    return istr;
}


#endif // INDIVIDUAL_HPP
