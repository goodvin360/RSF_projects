#include "RunAct.hh"
#include "iostream"
#include "map"
#include "CommandMessenger.hh"
#include "DetGeometry.hh"

using namespace std;

RunAct::RunAct(DetGeometry*det,PrimaryGen*gen):G4UserRunAction(),fGen(gen),fDetector(det)

{
    result = new std::map<G4double, G4int>;
    runactMessenger = new RunActMessenger(this);
}

RunAct::~RunAct()
{
    delete result;
    delete runactMessenger;
}

void RunAct::BeginOfRunAction(const G4Run *aRun)
{
    result->clear();
    for (int i=0; i <n_of_Bins; i++)
    result->insert(std::pair<G4double, G4int> (i *Emax / n_of_Bins, 0));
}


void RunAct::EndOfRunAction(const G4Run *aRun)
{
    if(!isMaster)
    {
        static G4Mutex myMutex = G4MUTEX_INITIALIZER;
        G4MUTEXLOCK(&myMutex);

        static auto test = new double[n_of_Bins]();

        for (int i = 0; i < n_of_Bins; i++) {
            test[i] += result->at(i * Emax / n_of_Bins);
        }

        static double num = 0;
        num++;
        if (num == G4Threading::GetNumberOfRunningWorkerThreads()) {
            stringstream res_out_mutex;
            res_out_mutex << "../res/Raw_Res.txt";
            std::ofstream myFile(res_out_mutex.str(), ios::out);

            for (int i = 0; i < n_of_Bins; i++) {
                myFile << i * Emax / n_of_Bins << '\t' << test[i] << endl;
                test[i] = 0;
            }
            num = 0;
        }
        G4MUTEXUNLOCK(&myMutex);
    }

    if(isMaster)
    {
        stringstream res_out_mutex;
        res_out_mutex << "../res/Raw_Res.txt";

        auto P = new double *[n_of_Bins];
        for (int i = 0; i < n_of_Bins; i++) {
            P[i] = new double[n_of_Bins]();}

        auto Sigma = new double[n_of_Bins]();
        auto EnergyScale = new double[n_of_Bins]();
        auto Counts = new double[n_of_Bins]();
        auto Counts_norm = new double[n_of_Bins]();
        auto Counts_pre = new double[n_of_Bins]();

        double E, Cnt;

        double row_sum, nrm1, nrm2;
        row_sum = 0;
        nrm1 = 0;
        nrm2 = 0;

        ifstream read_1;
        read_1.open(res_out_mutex.str());
        for (int i = 0; i < n_of_Bins; i++) {
            read_1 >> E >> Cnt;
            EnergyScale[i] = E;
            Counts[i] = Cnt;
        }
        read_1.close();

        Counts[0] = 0;
        EnergyScale[0] = 0.000001;

        double delta = EnergyScale[10]-EnergyScale[9];

        for (int i=0; i<n_of_Bins; i++)
        {Sigma[i] = 0.1405*(sqrt(EnergyScale[i]))/sqrt(1000);}
//        {Sigma[i] = 0.7025*(sqrt(EnergyScale[i]))/sqrt(1000);}


        for (int i = 0; i < n_of_Bins; i++)
        {
            for (int j = 0; j < n_of_Bins; j++)
            {
                P[i][j] = (delta*Counts[j] / (sqrt(2 * pi * pow(Sigma[i],2)))) *
                          exp(-1*(pow((EnergyScale[i] - EnergyScale[j]),2) /
                                  (2 * pow(Sigma[i],2))));
            }
        }

        for (int i = 0; i < n_of_Bins; i++)
        {
            for (int j = 0; j < n_of_Bins; j++)
            {
                row_sum += P[i][j];
            }
            Counts_pre[i] = row_sum;
            row_sum = 0;
        }

        for (int i=0; i<n_of_Bins; i++)
        {
            nrm1+=delta*Counts[i];
            nrm2+=delta*Counts_pre[i];
        }

        for (int i=0; i<n_of_Bins; i++)
        {
            Counts_norm[i]=(nrm1/nrm2)*Counts_pre[i];
        }

        stringstream res_out_norm;
        res_out_norm << "../res/Norm_Res.txt";
        fstream fout_norm(res_out_norm.str(), ios::out);
        for (int i = 0; i < n_of_Bins; i++)
        {
            if(Counts_norm[i]<1E-10)
            {Counts_norm[i]=0;}
            fout_norm << EnergyScale[i] << '\t' << Counts_norm[i] << '\n';
        }
        fout_norm.close();

        for (int i = 0; i < n_of_Bins; i++) { delete[] P[i]; }
        delete [] EnergyScale; delete [] Counts; delete [] Counts_norm;
        delete [] Counts_pre;  delete [] Sigma;
    }
}

void RunAct::AddEvent(G4double energy)
{
    auto it = result->lower_bound(energy);
    it->second++;
}

void RunAct::SetNewResPath(G4String newPath) {

    foldername = newPath;

}

