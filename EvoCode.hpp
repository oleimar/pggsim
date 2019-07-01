#ifndef EVOCODE_HPP
#define EVOCODE_HPP

#ifdef _OPENMP
#define PARA_RUN
#endif

#include "Genotype.hpp"
#include "Phenotype.hpp"
#include "Individual.hpp"
#include "MetaPopState.hpp"
#include "ACgroup.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

// An individual has 3 genetically determined traits, w0, theta0, d (see
// Phenotype.hpp), and there is one locus for each trait
const std::size_t NumLoci = 3;

using LocVec = std::array<double, NumLoci>;


//************************* Class EvoInpData ***************************

// This class is used to 'package' input data in a single place; the
// constructor extracts data from an input file

class EvoInpData {
public:
    std::size_t max_num_thrds;  // Max number of threads to use
    std::size_t nsp;            // Number of subpopulations
    std::size_t ngsp;           // Number of groups per subpopulation
    std::size_t g;              // Number of individuals in group
    std::size_t T;              // Number of time steps in group interaction
    std::size_t numgen;         // Number of generation to simulate
    double B0;                  // Parameter for reward
    double B1;                  // Parameter for reward
    double B2;                  // Parameter for reward
    double K1;                  // Parameter for reward
    double K11;                 // Parameter for reward
    double K12;                 // Parameter for reward
    double sigma;               // Standard deviation parameter for actions
    double alphaw;              // Learning rate parameter
    double alphatheta;          // Learning rate parameter
    double lambdatheta;         // Eligibility trace parameter
    int Nqv;                    // Number of values of q
    std::vector<double> qv;     // Vector of values of q
    bool ReadFromFile;          // Whether to read population from file
    bool cont_gen;              // Whether to continue in the first generation
    LocVec mut_rate;            // Probability of mutation at each locus
    LocVec SD;                  // SD of mutational increments at each locus
    LocVec max_val;             // Maximal allelic value at each locus
    LocVec min_val;             // Minimal allelic value at each locus
    LocVec rho;                 // Recombination rates
    LocVec all0;                // Starting allelic values (if not from file)
    std::string InName;         // File name for input of learning parameters
    std::string OutName;        // File name for output of learning parameters

    std::string InpName;  // Name of indata file
    bool OK;              // Whether indata has been successfully read

    EvoInpData(const char* filename);
};


//***************************** Class Evo ******************************

class Evo {
public:
    // types needed to define individual
    using mut_rec_type = MutRec<NumLoci, MutIncrNorm<>>;
    using gam_type = Gamete<NumLoci, mut_rec_type>;
    using gen_type = Diplotype<gam_type>;
    using phen_type = Phenotype<gen_type>;
    using ind_type = Individual<gen_type, phen_type>;
    // use simple subpopulation type, with payoff-based selection
    using subpop_type = SubPop0<ind_type>;
    using metapop_type = MetaPopState<subpop_type>;
    using vi_type = std::vector<ind_type>;
    using vph_type = std::vector<phen_type>;
    using acg_type = ActCritGroup<phen_type>;
    using i_type = std::vector<std::size_t>;
    using v_type = std::vector<double>;
    using i_pair = std::pair<std::size_t, std::size_t>;
    using rand_eng = std::mt19937;
    using rand_int = std::uniform_int_distribution<int>;
    using rand_uni = std::uniform_real_distribution<double>;
    using rand_norm = std::normal_distribution<double>;
    using rand_discr = std::discrete_distribution<int>;
    Evo(const EvoInpData& eid);
    void Run();
private:
    vi_type SelectReproduce(const subpop_type& sp, mut_rec_type& mr);
    i_pair spn_i(std::size_t n) { return i_pair(n / Ns, n % Ns); }

    EvoInpData id;
    std::size_t nsp;
    std::size_t ngsp;
    std::size_t g;
    std::size_t max_inds;
    std::size_t ng;
    std::size_t Ns;
    std::size_t N;
    std::size_t T;
    std::size_t numgen;
    double B0;
    double B1;
    double B2;
    double K1;
    double K11;
    double K12;
    double sigma;
    double alphaw;
    double alphatheta;
    double lambdatheta;
    int Nqv;
    v_type qv;
    std::size_t num_thrds;
    std::vector<unsigned> sds;
    bool popOK;
    metapop_type pop;
    metapop_type next_pop;
};

#endif // EVOCODE_HPP
