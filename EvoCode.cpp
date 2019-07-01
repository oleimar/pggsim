#include "EvoCode.hpp"
#include "InpFile.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <climits> // for UCHAR_MAX and UINT_MAX

#ifdef PARA_RUN
#include <omp.h>
#endif

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

//************************** Class EvoInpData ****************************

EvoInpData::EvoInpData(const char* filename) :
      OK(false)
{
    InpFile inp(filename);
    if ( !inp ) {
        std::cout << "Failed to open " << inp.GetFileName() << '\n';
        return;
    }
    Read(inp, max_num_thrds, "max_num_thrds");
    Read(inp, nsp, "nsp");
    Read(inp, ngsp, "ngsp");
    Read(inp, g, "g");
    Read(inp, T, "T");
    Read(inp, numgen, "numgen");
    Read(inp, B0, "B0");
    Read(inp, B1, "B1");
    Read(inp, B2, "B2");
    Read(inp, K1, "K1");
    Read(inp, K11, "K11");
    Read(inp, K12, "K12");
    Read(inp, sigma, "sigma");
    Read(inp, alphaw, "alphaw");
    Read(inp, alphatheta, "alphatheta");
    Read(inp, lambdatheta, "lambdatheta");
    Read(inp, Nqv, "Nqv");
    qv.resize(Nqv);
    ReadArr(inp, qv, "qv");
    ReadArr(inp, mut_rate, "mut_rate");
    ReadArr(inp, SD, "SD");
    ReadArr(inp, max_val, "max_val");
    ReadArr(inp, min_val, "min_val");
    ReadArr(inp, rho, "rho");
    Read(inp, ReadFromFile, "ReadFromFile");
    cont_gen = false;
    if ( ReadFromFile ) {
        Read(inp, cont_gen, "cont_gen");
        ReadString(inp, InName, "InName");
    } else {
        ReadArr(inp, all0, "all0");
    }
    ReadString(inp, OutName, "OutName");

    InpName = std::string(inp.GetFileName());
    OK = true;
}


//****************************** Class Evo *****************************

Evo::Evo(const EvoInpData& eid) :
    id{eid},
    nsp{id.nsp},
    ngsp{id.ngsp},
    g{id.g},
    max_inds{g*ngsp},
    ng{nsp*ngsp},
    Ns{ngsp*g},
    N{ng*g},
    T{id.T},
    numgen{id.numgen},
    B0{id.B0},
    B1{id.B1},
    B2{id.B2},
    K1{id.K1},
    K11{id.K11},
    K12{id.K12},
    sigma{id.sigma},
    alphaw{id.alphaw},
    alphatheta{id.alphatheta},
    lambdatheta{id.lambdatheta},
    Nqv{id.Nqv},
    qv{id.qv},
    num_thrds{1},
    sds(id.max_num_thrds),
    popOK{true},
    pop(nsp, max_inds),
    next_pop(nsp, max_inds)
{
    // decide on number of threads for parallel processing
    // (if PARA_RUN is undefined, the program is single-threaded)
#ifdef PARA_RUN
    num_thrds = omp_get_max_threads();
    if (num_thrds > id.max_num_thrds) num_thrds = id.max_num_thrds;
    // check that there is at least one subpopulation per thread
    if (num_thrds > nsp) num_thrds = nsp;
    std::cout << "Number of threads: "
              << num_thrds << '\n';
#endif
    // generate one seed for each thread
    std::random_device rd;
    for(int i = 0; i < num_thrds; ++i) {
        sds[i] = rd();
    }
    // check if population data should be read from file
    if (id.ReadFromFile) {
        popOK = pop.Read_from_File(id.InName, ng*g);
    } else {
        // construct all individuals as essentially the same
        gam_type gam(id.all0); // starting gamete
        ind_type ind(gam, 0);
        for (int n = 0; n < nsp; ++n) {
            ind.spn = n; // set subpopulation number
            subpop_type& sp = pop[n];
            sp.st.spn = n; // also set sp number (not really needed)
            next_pop[n].st.spn = n; // also for next_pop
            for (int k = 0; k < ngsp; ++k) {
                ind.phenotype.gnum = k + 1; // set group number
                for (int i = 0; i < g; ++i) { // add individuals to subpop
                    ind.phenotype.inum = i + 1; // set individual number
                    sp.Add(ind);
                }
            }
        }
    }
}

void Evo::Run()
{
    if (!popOK) {
        std::cout << "Starting population not valid \n";
        return;
    }
    Timer timer(std::cout);
    timer.Start();
    ProgressBar PrBar(std::cout, numgen);
#pragma omp parallel num_threads(num_thrds)
    {
#ifdef PARA_RUN
        int threadn = omp_get_thread_num();
#else
        int threadn = 0;
#endif
        // set up thread-local random number engine:
        rand_eng eng(sds[threadn]);
        rand_uni uni(0.0, 1.0);
        rand_int uri(0, Nqv - 1);
        rand_norm nrm(0.0, 1.0);
        // set up thread-local mutation record, with parameters controlling
        // mutation, segregation and recombination
        mut_rec_type mr(eng);
        // mr.SetRandEng(eng); // mr will use thread-local engine
        mr.mut_rate = id.mut_rate;
        mr.SD = id.SD;
        mr.max_val = id.max_val;
        mr.min_val = id.min_val;
        mr.rho = id.rho;
        // determine which subpopulations this thread should handle
        int num_per_thr = nsp/num_thrds;
        int NP1 = threadn*num_per_thr;
        int NP2 = NP1 + num_per_thr;
        if (threadn == num_thrds - 1) NP2 = nsp;
        // run through generations
        for (int gen = 0; gen < numgen; ++gen) {
            // set up (thread-local) MetaPopState object
            MetaPopState<subpop_type> popl(NP2 - NP1, max_inds);
            for (int n = NP1; n < NP2; ++n) {
                // transfer individuals from global to thread-local
                subpop_type& spg = pop[n];
                subpop_type& spl = popl[n - NP1];
                // set subpopulation number of local
                spl.st.spn = n;
                for (int i = 0; i < spg.size(); ++i) {
                    spl.Add(spg[i]);
                }
                if (gen > 0 || !id.cont_gen ) {
                    // assign (random) quality values
                    for (int i = 0; i < spl.size(); ++i) {
                        phen_type& ph = spl[i].phenotype;
                        ph.q = qv[uri(eng)];
                        ph.p = ph.q + ph.d;
                    }
                }
                // set up interaction groups, interact and get data
                for (int k = 0; k < ngsp; ++k) {
                    vph_type phen(g);
                    for (int j = 0; j < g; ++j) {
                        int i = k*g + j;
                        phen[j] = spl[i].phenotype;
                    }
                    acg_type acg(g, T, B0, B1, B2, K1, K11, K12, sigma,
                                 alphaw, alphatheta, lambdatheta, phen);
                    acg.Interact(eng);
                    const vph_type& memb = acg.Get_memb();
                    for (int j = 0; j < g; ++j) {
                        int i = k*g + j;
                        spl[i].phenotype = memb[j];
                    }
                }
// #pragma omp critical
                // this section is not really critical, because each thread
                // writes to different subpopulations next_pop[n] (or pop[n])
                if (gen < numgen - 1) {
                    // if not final generation, get offspring from this
                    // subpopulation and put into next_pop
                    subpop_type& next_spg = next_pop[n];
                    next_spg.clear();
                    next_spg.ind = SelectReproduce(spl, mr);
                } else {
                    for (int i = 0; i < spg.size(); ++i) {
                        spg[i] = spl[i];
                    }
                    // set subpopulation number
                    spg.st.spn = n;
                }
            }
#pragma omp barrier
            if (threadn == 0 && gen < numgen - 1) {
                // if not final generation, transfer all individuals to random
                // position in pop, for start of next generation; the code
                // below copies a random individual from next_pop for each
                // position in pop
                i_type indx(N, 0);
                for (std::size_t n = 0; n < N; ++n) {
                    indx[n] = n;
                }
                // construct positions, 0 to N-1, of "random individuals"
                std::shuffle(indx.begin(), indx.end(), eng);
                std::size_t n = 0;
                for (std::size_t spn = 0; spn < nsp; ++spn) {
                    subpop_type& sp = pop[spn];
                    for (int k = 0; k < ngsp; ++k) {
                        int gnum = k + 1; // group number
                        for (int j = 0; j < g; ++j) {
                            int inum = j + 1;
                            int i = k*g + j;
                            // construct position in next_pop corresponding to
                            // n for "random individual"
                            i_pair n_i = spn_i(indx[n++]);
                            sp[i] = next_pop[n_i.first][n_i.second];
                            // update spn, gnum, inum for copied individual
                            sp[i].spn = spn;
                            sp[i].phenotype.gnum = gnum;
                            sp[i].phenotype.inum = inum;
                        }
                    }
                }
                // all set to start next generation
                ++PrBar;
            }
        }
    }
    PrBar.Final();
    timer.Stop();
    timer.Display();
    pop.Write_to_File(id.OutName);
}

// return vector of Ns offspring from the subpopulation in sp, with individual
// payoff being proportional to the probability of delivering a gamete, and
// using mutation and recombination parameters from mr
Evo::vi_type Evo::SelectReproduce(const subpop_type& sp, mut_rec_type& mr)
{
    vi_type offspr;
    offspr.reserve(Ns);
    std::size_t np = sp.NumInds();
    if (np > 0) {
        // get discrete distribution with parental payoffs as weights
        v_type wei(np);
        for (int i = 0; i < np; ++i) {
            wei[i] = sp[i].phenotype.payoff;
        }
        rand_discr dscr(wei.begin(), wei.end());
        // subpopulation number
        std::size_t spn = sp.GetState().spn;
        // get offspring
        for (int j = 0; j < Ns; ++j) {
            // find "mother" for individual to be constructed
            std::size_t imat = dscr(mr.eng);
            const ind_type& matind = sp[imat];
            // find "father" for individual to be constructed
            std::size_t ipat = dscr(mr.eng);
            const ind_type& patind = sp[ipat];
            // append new individual to offspr
            offspr.emplace_back(matind.GetGamete(mr),
                                patind.GetGamete(mr), spn);
        }
    }
    return offspr;
}
