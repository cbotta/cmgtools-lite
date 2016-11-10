#include <cmath>
#include <map>
#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#include "Math/GenVector/PxPyPzM4D.h"
#include "Math/GenVector/Boost.h"
#include "TLorentzVector.h"
#include "TH2Poly.h"
#include "TGraphAsymmErrors.h"
#include "TH1F.h"
#include "TFile.h"
#include "PhysicsTools/Heppy/interface/Davismt2.h"
#include "TSystem.h"

TString CMSSW_BASE = gSystem->ExpandPathName("${CMSSW_BASE}");

//// UTILITY FUNCTIONS NOT IN TFORMULA ALREADY

float myratio(float num, float denom) {
  if(denom==0) return 0;
  return num/denom;
}

float deltaPhi(float phi1, float phi2) {
    float result = phi1 - phi2;
    while (result > float(M_PI)) result -= float(2*M_PI);
    while (result <= -float(M_PI)) result += float(2*M_PI);
    return result;
}

float if3(bool cond, float iftrue, float iffalse) {
    return cond ? iftrue : iffalse;
}

float deltaR2(float eta1, float phi1, float eta2, float phi2) {
    float deta = std::abs(eta1-eta2);
    float dphi = deltaPhi(phi1,phi2);
    return deta*deta + dphi*dphi;
}
float deltaR(float eta1, float phi1, float eta2, float phi2) {
    return std::sqrt(deltaR2(eta1,phi1,eta2,phi2));
}

float pt_2(float pt1, float phi1, float pt2, float phi2) {
    phi2 -= phi1;
    return hypot(pt1 + pt2 * std::cos(phi2), pt2*std::sin(phi2));
}

float mt_2(float pt1, float phi1, float pt2, float phi2) {
    return std::sqrt(2*pt1*pt2*(1-std::cos(phi1-phi2)));
}

float mass_2(float pt1, float eta1, float phi1, float m1, float pt2, float eta2, float phi2, float m2) {
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PtEtaPhiMVector;
    PtEtaPhiMVector p41(pt1,eta1,phi1,m1);
    PtEtaPhiMVector p42(pt2,eta2,phi2,m2);
    return (p41+p42).M();
}

float mt2davis(float pt1, float eta1, float phi1, float pt2, float eta2, float phi2, float met, float metphi){
    // NOTE THAT THIS FUNCTION ASSUMES MASSLESS OBJECTS. NOT ADVISED TO USE WITH HEMISPHERES ETC.
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PtEtaPhiMVector;
    PtEtaPhiMVector p1(pt1,eta1,phi1,0.);
    PtEtaPhiMVector p2(pt2,eta2,phi2,0.);
    PtEtaPhiMVector mv(met,0.,metphi,0.);
    double a[] = {p1.M(), p1.Px(), p1.Py()};
    double b[] = {p2.M(), p2.Px(), p2.Py()};
    double c[] = {mv.M(), mv.Px(), mv.Py()};

    heppy::Davismt2 mt2obj;
    mt2obj.set_momenta( a, b, c );
    mt2obj.set_mn( 0. );

    float result = (float) mt2obj.get_mt2();
    return result;
}

float phi_2(float pt1, float phi1, float pt2, float phi2) {
    float px1 = pt1 * std::cos(phi1);
    float py1 = pt1 * std::sin(phi1);
    float px2 = pt2 * std::cos(phi2);
    float py2 = pt2 * std::sin(phi2);
    return std::atan2(py1+py2,px1+px2);
}

float pt_3(float pt1, float phi1, float pt2, float phi2, float pt3, float phi3) {
    phi2 -= phi1;
    phi3 -= phi1;
    return hypot(pt1 + pt2 * std::cos(phi2) + pt3 * std::cos(phi3), pt2*std::sin(phi2) + pt3*std::sin(phi3));
}


float mass_3(float pt1, float eta1, float phi1, float m1, float pt2, float eta2, float phi2, float m2, float pt3, float eta3, float phi3, float m3) {
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PtEtaPhiMVector;
    PtEtaPhiMVector p41(pt1,eta1,phi1,m1);
    PtEtaPhiMVector p42(pt2,eta2,phi2,m2);
    PtEtaPhiMVector p43(pt3,eta3,phi3,m3);
    return (p41+p42+p43).M();
}


float pt_4(float pt1, float phi1, float pt2, float phi2, float pt3, float phi3, float pt4, float phi4) {
    phi2 -= phi1;
    phi3 -= phi1;
    phi4 -= phi1;
    return hypot(pt1 + pt2 * std::cos(phi2) + pt3 * std::cos(phi3) + pt4 * std::cos(phi4), pt2*std::sin(phi2) + pt3*std::sin(phi3) + pt4*std::sin(phi4));
}
 
float mass_4(float pt1, float eta1, float phi1, float m1, float pt2, float eta2, float phi2, float m2, float pt3, float eta3, float phi3, float m3, float pt4, float eta4, float phi4, float m4) {
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PtEtaPhiMVector;
    PtEtaPhiMVector p41(pt1,eta1,phi1,m1);
    PtEtaPhiMVector p42(pt2,eta2,phi2,m2);
    PtEtaPhiMVector p43(pt3,eta3,phi3,m3);
    PtEtaPhiMVector p44(pt4,eta4,phi4,m4);
    return (p41+p42+p43+p44).M();
}

float mt_llv(float ptl1, float phil1, float ptl2, float phil2, float ptv, float phiv) {
    float px = ptl1*std::cos(phil1) + ptl2*std::cos(phil2) + ptv*std::cos(phiv);
    float py = ptl1*std::sin(phil1) + ptl2*std::sin(phil2) + ptv*std::sin(phiv);
    float ht = ptl1+ptl2+ptv;
    return std::sqrt(std::max(0.f, ht*ht - px*px - py*py));
}

float mt_lllv(float ptl1, float phil1, float ptl2, float phil2, float ptl3, float phil3, float ptv, float phiv) {
    float px = ptl1*std::cos(phil1) + ptl2*std::cos(phil2) + ptl3*std::cos(phil3) + ptv*std::cos(phiv);
    float py = ptl1*std::sin(phil1) + ptl2*std::sin(phil2) + ptl3*std::sin(phil3) + ptv*std::sin(phiv);
    float ht = ptl1+ptl2+ptl3+ptv;
    return std::sqrt(std::max(0.f, ht*ht - px*px - py*py));
}


float mtw_wz3l(float pt1, float eta1, float phi1, float m1, float pt2, float eta2, float phi2, float m2, float pt3, float eta3, float phi3, float m3, float mZ1, float met, float metphi) 
{
    if (abs(mZ1 - mass_2(pt1,eta1,phi1,m1,pt2,eta2,phi2,m2)) < 0.01) return mt_2(pt3,phi3,met,metphi);
    if (abs(mZ1 - mass_2(pt1,eta1,phi1,m1,pt3,eta3,phi3,m3)) < 0.01) return mt_2(pt2,phi2,met,metphi);
    if (abs(mZ1 - mass_2(pt2,eta2,phi2,m2,pt3,eta3,phi3,m3)) < 0.01) return mt_2(pt1,phi1,met,metphi);
    return 0;
}

float u1_2(float met_pt, float met_phi, float ref_pt, float ref_phi) 
{
    float met_px = met_pt*std::cos(met_phi), met_py = met_pt*std::sin(met_phi);
    float ref_px = ref_pt*std::cos(ref_phi), ref_py = ref_pt*std::sin(ref_phi);
    float ux = - met_px + ref_px, uy = - met_px + ref_px;
    return (ux*ref_px + uy*ref_py)/ref_pt;
}
float u2_2(float met_pt, float met_phi, float ref_pt, float ref_phi)
{
    float met_px = met_pt*std::cos(met_phi), met_py = met_pt*std::sin(met_phi);
    float ref_px = ref_pt*std::cos(ref_phi), ref_py = ref_pt*std::sin(ref_phi);
    float ux = - met_px + ref_px, uy = - met_px + ref_px;
    return (ux*ref_py - uy*ref_px)/ref_pt;
}

// reconstructs a top from lepton, met, b-jet, applying the W mass constraint and taking the smallest neutrino pZ
float mtop_lvb(float ptl, float etal, float phil, float ml, float met, float metphi, float ptb, float etab, float phib, float mb) 
{
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PtEtaPhiMVector;
    typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > PxPyPzMVector;
    PtEtaPhiMVector p4l(ptl,etal,phil,ml);
    PtEtaPhiMVector p4b(ptb,etab,phib,mb);
    double MW=80.4;
    double a = (1 - std::pow(p4l.Z()/p4l.E(), 2));
    double ppe    = met * ptl * std::cos(phil - metphi)/p4l.E();
    double brk    = MW*MW / (2*p4l.E()) + ppe;
    double b      = (p4l.Z()/p4l.E()) * brk;
    double c      = met*met - brk*brk;
    double delta   = b*b - a*c;
    double sqdelta = delta > 0 ? std::sqrt(delta) : 0;
    double pz1 = (b + sqdelta)/a, pz2 = (b - sqdelta)/a;
    double pznu = (abs(pz1) <= abs(pz2) ? pz1 : pz2);
    PxPyPzMVector p4v(met*std::cos(metphi),met*std::sin(metphi),pznu,0);
    return (p4l+p4b+p4v).M();
}

float DPhi_CMLep_Zboost(float l_pt, float l_eta, float l_phi, float l_M, float l_other_pt, float l_other_eta, float l_other_phi, float l_other_M){
  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PtEtaPhiMVector;
  PtEtaPhiMVector l1(l_pt,l_eta,l_phi,l_M);
  PtEtaPhiMVector l2(l_other_pt,l_other_eta,l_other_phi,l_other_M);
  PtEtaPhiMVector Z = l1+l2;
  ROOT::Math::Boost boost(Z.BoostToCM());
  l1 = boost*l1;
  return deltaPhi(l1.Phi(),Z.Phi());
}

float relax_cut_in_eta_bins(float val, float eta, float eta1, float eta2, float eta3, float val1, float val2, float val3, float val1t, float val2t, float val3t){

// Return a new value of val (variable on which a cut is applied), in such a way that the thresholds (val1,val2,val3)
// initially valid in regions of abs(eta)<(eta1,eta2,eta3) become effectively (val1t,val2t,val3t).
// The cut must be of the form val>=valN, and the condition valNt>valN must hold.

  if (abs(eta)<eta1){
    if (val>=val1) return val1t;
  }
  else if (abs(eta)<eta2){
    if (val>=val2) return val2t;
  }
  else if (abs(eta)<eta3){
    if (val>=val3) return val3t;
  }
  return val;

}



//PU weights

// for 74X
//float _puw_true[50] = {3.652322599922302, 3.652322599922302, 3.652322599922302, 3.652322599922302, 3.652322599922302, 3.652322599922302, 2.1737862420968868, 2.7116925849897364, 3.352556070095877, 3.083015137131128, 2.8824218072960823, 2.6791975503716743, 2.212434153800565, 1.5297063638539434, 0.8762698648562287, 0.41326633649647065, 0.17496252648670657, 0.07484562496757297, 0.038507396968229766, 0.021849761893692053, 0.01140425609526747, 0.005063578526248854, 0.001881351382104846, 0.0006306639125313864, 0.00021708627575927402, 9.42187694469501e-05, 5.146591433045169e-05, 3.326854405002371e-05, 2.426063215708668e-05, 1.8575279862433386e-05, 1.281054551977887e-05, 8.01819566777096e-06, 3.6521122159066883e-06, 1.574921039309069e-06, 5.770182058345157e-07, 2.0862027190449754e-07, 6.946502045299735e-08, 2.032077576113469e-08, 6.417943581326451e-09, 1.5934414668326278e-09, 4.311337237072122e-10, 1.1138367777447038e-10, 2.6925919137965106e-11, 8.08827951069873e-12, 1.3708268591386723e-12, 4.065021195897016e-13, 1.770006195676463e-13, 5.689967214903059e-14, 6.224880123134382e-14, 0.0};
//
float _puw_Mu8[60] = { 1.0, 2.2753238054533287, 2.5087852081376565, 2.547534409862272, 2.5453856677292626, 2.3957671901210302, 2.329888347258754, 2.132632821246379, 1.9294351293836975, 1.6869299027848321, 1.410414475296686, 1.18061883859625, 0.9505511419155187, 0.7455821522790507, 0.5917331920192597, 0.4497875190879285, 0.3529109417225823, 0.26485881058611677, 0.208008048447543, 0.1582531319686015, 0.12944461423876671, 0.10248826819473823, 0.07759715482270031, 0.05888999109148935, 0.059779044174240605, 0.041874820142546565, 0.03324024895102297, 0.039028818938047924, 0.02179308514266076, 0.01197215401539325, 0.009421343447104134, 0.013861014595193729, 0.0, 0.06539755302753723, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
//float _puw_Mu17[60] = { 1.0, 2.6025657030947866, 2.8330872498461876, 2.477197375267561, 2.552415607786326, 2.4047951682107627, 2.3024376807492737, 2.1540414886183843, 1.9518423032680638, 1.699049184347834, 1.4639137707975747, 1.2067661580794682, 0.966660952828535, 0.766250445622546, 0.5938409557059098, 0.4557404537508868, 0.3447511898010737, 0.26427563682379623, 0.2033725709370245, 0.15418157724654288, 0.12117494844707002, 0.09683984946855388, 0.08058574696446474, 0.06008103415479263, 0.049369064099306637, 0.043438226650837304, 0.032177902255824065, 0.029449084226433955, 0.02071379137395143, 0.01793424569381745, 0.013294248051062496, 0.012108360448835863, 0.009083026618738383, 0.0091627022908326, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
//

// for 76X
float _puw_true[50] = {0.5269234785559587, 0.6926695654592266, 1.080004829553802, 1.379105403255213, 1.819531519703243, 2.212948765398841, 1.591693852037577, 1.3246151544739415, 1.332963823519763, 1.252466214103506, 1.1890944076884082, 1.1546479191424708, 1.0968414518989904, 0.9606172752657895, 0.7612068106342116, 0.5447695822686637, 0.373924851932163, 0.27930793003309273, 0.24795243534715622, 0.2166027139283378, 0.1428710628614267, 0.06602338110497821, 0.021980743935920363, 0.006398279132023322, 0.0017936194597459384, 0.0006009500899246867, 0.00024768297660153493, 0.00011533171910677974, 5.940474255383892e-05, 3.0175857564910636e-05, 1.6223458431650605e-05, 8.845524938467253e-06, 4.8328429446258596e-06, 2.435353948121383e-06, 1.1669731534545345e-06, 5.843672017481929e-07, 2.975267432326348e-07, 1.3319634853069228e-07, 4.880943490079542e-08, 1.8002534751187506e-08, 1.2438479691035926e-08, 5.881024897175279e-09, 1.559130111354315e-09, 4.399999147793046e-10, 5.222167998741082e-11, 2.6387645368250217e-11, 1.0, 1.0, 1.0, 1.0};
//
//float puw(int nTrueInt) { if (nTrueInt<50) return _puw_true[nTrueInt]; else return 0; }


// CAREFUL to the bin alignment: check which bin is nVert==0
// for 80X miniAOD v2, golden up to run 274443 (2.6/fb, Jun16 JSON)
float _puw2016_vtx[60] = {1.0, 0.05279044032209489, 0.11898234374437847, 0.24839403240247732, 0.4414409758143405, 0.6800191525008986, 0.911238430838953, 1.1249250544346439, 1.3083391445128427, 1.4527031750771802, 1.5156438062395463, 1.5389169972670533, 1.5035916926401598, 1.4373269045147126, 1.333092377737066, 1.2248748489799457, 1.0918319339782905, 0.9636778613250641, 0.8410627330851944, 0.7261267863768212, 0.6235739762642099, 0.5315059034719868, 0.44970258176095207, 0.3828912199699739, 0.32480952469934127, 0.27387410135800994, 0.23077463619648667, 0.1962917069010862, 0.16875803152705662, 0.14725031730832552, 0.13147828517394833, 0.11963481956991436, 0.1055294146473766, 0.09910459281290813, 0.08749009279742694, 0.07566616130096912, 0.07414888388958515, 0.07427197742439709, 0.07831177286837666, 0.07210793417378795, 0.14541998285529517, 0.05653406075048574, 0.053526929859502484, 0.2147604868753209, 0.1006306281358646, 0.2515765703396614, 0.0, 1.2578828516983072, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0};
//float puw2016_vtx(int nVtx) { if (nVtx<60) return _puw2016_vtx[nVtx]; else return 0; }

// for up to 275125
float _puw2016_vtx_4fb[60] = {1.0, 0.05100939489406209, 0.10753683801507614, 0.22479210497504293, 0.4061194548857097, 0.6424861757674111, 0.8699208198466813, 1.0886008554762319, 1.276892560891215, 1.4184863274330797, 1.4865216108423305, 1.5122068499515893, 1.4790079191970888, 1.4196901839857483, 1.3216638995044172, 1.2217703342251016, 1.0976458888260794, 0.9796814642869479, 0.8639064021650553, 0.7535695441176858, 0.6555461235682296, 0.5641253790572658, 0.4834647260351342, 0.4164426860491679, 0.35606001606142423, 0.30497669791734305, 0.2592054182625056, 0.22335323942907434, 0.19192071849647147, 0.1672517813653499, 0.14683806760557278, 0.13614259650259863, 0.11857021273678048, 0.11205636298564389, 0.10212521715621654, 0.08684353771752344, 0.08864604848027625, 0.09804676288437263, 0.09235565349796869, 0.07943403772193891, 0.14417507424675546, 0.08136302667712911, 0.05135680407279777, 0.1569931571656257, 0.12873438887581296, 0.3218359721895326, 0.1609179860947663, 0.8045899304738313, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0 };
float puw2016_vtx_4fb(int nVtx) { if (nVtx<60) return _puw2016_vtx_4fb[nVtx]; else return 0; }

// for runs 274954-275125
float _puw2016_vtx_postTS_1p4fb[60] = {1.0, 0.0436628547984852, 0.0842489361361326, 0.17821945962362193, 0.3359458176736438, 0.5602926533570876, 0.7949986470183944, 1.009956832499392, 1.2171719031951709, 1.3352425499300327, 1.440211942752969, 1.461874846281358, 1.442598177264856, 1.3933114880260349, 1.303895089233208, 1.2175329549318987, 1.1119038703640942, 1.0164455216655062, 0.9118520665638973, 0.8059572090329304, 0.7155624000589931, 0.6289173123131537, 0.551810715512236, 0.4833237093643216, 0.4192621566956886, 0.3644494058922173, 0.30458903411821625, 0.2707118142127595, 0.2278811064155144, 0.20083759822187328, 0.17416091877224055, 0.16340944020160908, 0.14995476800202442, 0.12504239103683595, 0.12132829462778992, 0.09634514689836478, 0.1269739863549877, 0.1477893939541658, 0.12321393664515852, 0.03694734848854124, 0.10072796682909534, 0.06010102020802678, 0.11268941289005037, 0.0, 0.20488984161827345, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0};
float puw2016_vtx_postTS_1p4fb(int nVtx) { if (nVtx<60) return _puw2016_vtx_postTS_1p4fb[nVtx]; else return 0; }


// for json up to 276384 (9.235/fb)
float _puw2016_vtx_9fb[60] = {1.0, 0.04935539210063196, 0.10330050968638685, 0.2147911101772578, 0.39265726267153417, 0.6154543185021981, 0.8307112462687154, 1.0234405172528802, 1.1925131512920364, 1.2992751918824221, 1.3675693415474597, 1.383571620896604, 1.370751371914269, 1.3312898916887232, 1.260970719202342, 1.1934158915388204, 1.1079581139187757, 1.0207671452254607, 0.9330680851961782, 0.8459974039502247, 0.7660997972524449, 0.6853105592363634, 0.619098569695837, 0.5548227688124637, 0.498590328109847, 0.43969852956854144, 0.39485106899051653, 0.3546221583391258, 0.32414310290951415, 0.29492937650181855, 0.26816973928662186, 0.26026228952267894, 0.25438200170624503, 0.24305609489401844, 0.2402861143189915, 0.22410752071130485, 0.2329521554125791, 0.27204322650384044, 0.2187522991708468, 0.2697045149716429, 0.26325149847967466, 0.20939084706659114, 0.41327140868406215, 0.45300904413445264, 0.15704313529994357, 1.0, 0.3926078382498588, 1.963039191249294, 0.39260783824985873, 1.1778235147495764, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0};
float puw2016_vtx_9fb(int nVtx) { if (nVtx<60) return _puw2016_vtx_9fb[nVtx]; else return 0; }


// for json up to 276811 (12.9/fb)
float _puw2016_vtx_13fb[60] = {1.0, 0.046904649804193066, 0.09278031810949669, 0.18880389403907694, 0.3514757265099305, 0.557758357976481, 0.7693577917575528, 0.9666548740765918, 1.145319485841941, 1.2648398335691222, 1.3414360633779425, 1.3679594451533137, 1.362759399034107, 1.327376308549365, 1.2613315166803767, 1.196440614259811, 1.1139701579261285, 1.029953473266092, 0.9499371225384508, 0.8650456207995321, 0.7851579627730857, 0.7104602883630896, 0.6454503663312138, 0.5827160708961265, 0.527376483837914, 0.4700331217669938, 0.4271753119677936, 0.3869926520067443, 0.35986269245880403, 0.3280226115374019, 0.2995626735821264, 0.29695297220375283, 0.2904602474734967, 0.27797348557821827, 0.27285575884404983, 0.2696769830193652, 0.2834280746705423, 0.3079991295527812, 0.2958183730167929, 0.3281547943587132, 0.34428579006474574, 0.34709355767303973, 0.5916367460335905, 0.4991935044658422, 0.2689257936516321, 1.0, 0.690242870372522, 2.0707286111175662, 0.2958183730167952, 0.8874551190503855, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0};
float puw2016_vtx_13fb(int nVtx) { if (nVtx<60) return _puw2016_vtx_13fb[nVtx]; else return 0; }

// for json up to 276811 (12.9/fb), pu true reweighting
float _puw2016_nTrueInt_13fb[60] = {0.0004627598152210959, 0.014334910915287028, 0.01754727657726197, 0.03181477917631854, 0.046128282569231016, 0.03929080994013006, 0.057066019809589925, 0.19570744862221007, 0.3720256062526554, 0.6440076202772811, 0.9218024454406528, 1.246743510634073, 1.5292543296414058, 1.6670061646418215, 1.7390553377117133, 1.6114721876895595, 1.4177294439817985, 1.420132866045718, 1.3157656415540477, 1.3365188060918483, 1.1191478126677334, 0.9731079434848392, 0.9219564145009487, 0.8811793391804676, 0.7627315352977334, 0.7265186492688713, 0.558602385324645, 0.4805954159733825, 0.34125298049234554, 0.2584848657646724, 0.1819638766151892, 0.12529545619337035, 0.11065705912071645, 0.08587356267495487, 0.09146322371620583, 0.11885517671051576, 0.1952483711863489, 0.23589115679998116, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
float puw2016_nTrueInt_13fb(int nTrueInt) { if (nTrueInt<60) return _puw2016_nTrueInt_13fb[nTrueInt]; else return 0; }

// Run2016 E+F+G up to 279931
float _puw2016_nTrueInt_EF[60] = {0.0002476692378524667, 0.014821369430223724, 0.01908474123097549, 0.018774416942744378, 0.027731029726566526, 0.022766323438142735, 0.02112794605705192, 0.015627067131392624, 0.013027012984949482, 0.02664421891070537, 0.105951322567897, 0.2282219493320249, 0.3198539045796174, 0.4472658025206919, 0.6588805154724832, 0.7954191125076755, 0.7971828693076433, 0.8476493223752923, 0.828823661825391, 0.9020575899440882, 0.8251811240056177, 0.8088819151700594, 0.898945222693826, 1.0499718952630142, 1.1569874762934436, 1.4559615822558385, 1.5258279117119538, 1.8409115119643453, 1.8896981359803084, 2.140209911977273, 2.3304811410048387, 2.558753854290941, 3.6974162747530404, 4.798085511876458, 5.7327747190694405, 5.7327747190694405, 5.7327747190694405, 5.7327747190694405, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993, 1.0318994494324993};
float _puw2016_nTrueInt_G_upto279931[60] = {0.0030664708563046326, 0.01123794331581895, 0.016465056588825536, 0.024602832177926543, 0.026107749406095258, 0.019050503081821886, 0.02110161688725923, 0.017694605650846847, 0.016290999589588673, 0.03017245306638949, 0.062201175397462435, 0.21783085244166905, 0.5552753191757233, 0.7214955162768668, 0.7324485837208575, 0.7293440437992165, 0.7146681538266634, 0.7527382158346893, 0.7180480259007131, 0.7910622869835229, 0.7597422022206722, 0.7786794509943739, 0.8873764384591624, 1.0499884204081869, 1.1665585452761693, 1.482186697601494, 1.579761549660652, 1.9529819332323881, 2.053483793295554, 2.356853951860021, 2.5540748462771266, 2.7323308232944745, 3.76828118550158, 4.585241234971839, 5.680781421298886, 5.680781421298886, 5.680781421298886, 5.680781421298886, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995, 1.0225406558337995};
float _puw2016_nTrueInt_EFG_upto279931[60] = {0.0012466094493921048, 0.013160935994534766, 0.01768708902983447, 0.021285749046576714, 0.02682365314703701, 0.020855099389744048, 0.020605061123741098, 0.016225941568981307, 0.014048132697170798, 0.02723810756281671, 0.08766748594327597, 0.2188586889136281, 0.3959226446275465, 0.53375637601322, 0.668298874408685, 0.7522202102811592, 0.7512257707056236, 0.8045384529297095, 0.790321346838139, 0.8694715549669896, 0.80985012101077, 0.8077764909536846, 0.9071025263393598, 1.0638089303658826, 1.17351814974311, 1.478754892106334, 1.5559650574410981, 1.8900984223481412, 1.9531327854179075, 2.2173732038373344, 2.402868150348858, 2.6038209490463284, 3.685234828496249, 4.657038942103952, 5.7091311002033045, 5.7091311002033045, 5.7091311002033045, 5.7091311002033045, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948, 1.0276435980365948};
float puw2016_nTrueInt_EF(int nTrueInt) { if (nTrueInt<60) return _puw2016_nTrueInt_EF[nTrueInt]; else return 0; }
float puw2016_nTrueInt_G_upto279931(int nTrueInt) { if (nTrueInt<60) return _puw2016_nTrueInt_G_upto279931[nTrueInt]; else return 0; }
float puw2016_nTrueInt_EFG_upto279931(int nTrueInt) { if (nTrueInt<60) return _puw2016_nTrueInt_EFG_upto279931[nTrueInt]; else return 0; }

//
//float puwMu8(int nVert) { return _puw_Mu8[nVert] * 0.001; }
//float puwMu17(int nVert) { return _puw_Mu17[nVert] * (2305428/29339.)*0.002/2.26; }

TFile* puw2016_ICHEP = NULL;
TFile* puw2016_ICHEP_Up = NULL;
TFile* puw2016_ICHEP_Dn = NULL;
TH1F* _puw2016_nInt_ICHEP = NULL;
TH1F* _puw2016_nInt_ICHEP_Up = NULL;
TH1F* _puw2016_nInt_ICHEP_Dn = NULL;
float puw2016_nInt_ICHEP(float nInt, int var=0) { 
  
  if (var==0) { 
    if (!_puw2016_nInt_ICHEP){ 
      puw2016_ICHEP = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/pileup/puWeights_12fb_63mb.root", "read");
      _puw2016_nInt_ICHEP = (TH1F*) (puw2016_ICHEP->Get("puw"));
    }
    return _puw2016_nInt_ICHEP->GetBinContent(_puw2016_nInt_ICHEP->FindBin(nInt));
  }
  else if (var==1) { 
    if (!puw2016_ICHEP_Up) {
      puw2016_ICHEP_Up = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/pileup/puWeights_12fb_63mb_Up.root", "read");
      _puw2016_nInt_ICHEP_Up = (TH1F*) (puw2016_ICHEP_Up->Get("puw"));
    }
    return _puw2016_nInt_ICHEP_Up->GetBinContent(_puw2016_nInt_ICHEP_Up->FindBin(nInt));
  }
  else if (var==-1) {
    if (!puw2016_ICHEP_Dn) {
      puw2016_ICHEP_Dn = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/pileup/puWeights_12fb_63mb_Down.root", "read");
      _puw2016_nInt_ICHEP_Dn = (TH1F*) (puw2016_ICHEP_Dn->Get("puw"));
    }
    return _puw2016_nInt_ICHEP_Dn->GetBinContent(_puw2016_nInt_ICHEP_Dn->FindBin(nInt));
  }
  cout <<"[WARNING!!!]  don't know what to do with PUweight, please check!! ";
  return -9999.;
}


float _puw2016_nInt_4fb[50] = {0.000541449, 0.0053609, 0.0129558, 0.0284204, 0.0485405, 0.0381546, 0.0455114, 0.201159, 0.417699, 0.551746, 0.708713, 0.948842, 1.26172, 1.63146, 1.95588, 1.93643, 1.75927, 1.77082, 1.58969, 1.54663, 1.21777, 0.97649, 0.830633, 0.699876, 0.521962, 0.416445, 0.260446, 0.172585, 0.0916121, 0.0502204, 0.0253793, 0.0122719, 0.00776576, 0.00462153, 0.00427825, 0.00593001, 0.0137304, 0.0306841, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float puw2016_nInt_4fb(int nInt) { if(nInt<50) return _puw2016_nInt_4fb[nInt]; else return 0; }

float _puw2016_nInt_6p3fb[50] = {0.000382638, 0.00904568, 0.0172266, 0.028787, 0.0480456, 0.0395218, 0.0455648, 0.168451, 0.383086, 0.648308, 0.936192, 1.1818, 1.3745, 1.57972, 1.78097, 1.74766, 1.60982, 1.64109, 1.49706, 1.4961, 1.21978, 1.01628, 0.897894, 0.782684, 0.602069, 0.498483, 0.329922, 0.238593, 0.14344, 0.0926828, 0.0574109, 0.035114, 0.0284437, 0.0210839, 0.0219857, 0.027923, 0.0455562, 0.0608525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float puw2016_nInt_6p3fb(int nInt) { if(nInt<50) return _puw2016_nInt_6p3fb[nInt]; else return 0; }



#include <assert.h>
#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"

//float _puw2016_nInt_9p2fb[200] = {2.75256e-07, 0.000513282, 0.00244189, 0.0137388, 0.0107657, 0.0170963, 0.019015, 0.0318117, 0.0414126, 0.0475017, 0.031669, 0.0357413, 0.0311912, 0.0431186, 0.0772747, 0.175901, 0.243574, 0.413217, 0.517095, 0.745812, 0.783944, 0.988148, 1.01437, 1.15628, 1.15795, 1.33804, 1.32247, 1.49758, 1.4432, 1.59987, 1.42034, 1.54635, 1.34517, 1.42764, 1.46384, 1.47524, 1.40116, 1.4, 1.49124, 1.45276, 1.26607, 1.22946, 1.14149, 1.0768, 1.10811, 1.01191, 1.05536, 0.951295, 0.916776, 0.798908, 0.867608, 0.721483, 0.638217, 0.513803, 0.521847, 0.406786, 0.346672, 0.26268, 0.244442, 0.181843, 0.161703, 0.115209, 0.103819, 0.0724423, 0.0865247, 0.0562273, 0.064715, 0.0418875, 0.0663075, 0.0419478, 0.0746418, 0.047408, 0.127639, 0.0720651, 0.169214, 0.0774973, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


TFile* puw9p2fb = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/pileup/puw_2016_9fb.root", "read");
TH1F* _puw2016_nInt_9p2fb = (TH1F*) puw9p2fb->Get("puw");

float puw2016_nInt_9p2fb(float nInt) { return _puw2016_nInt_9p2fb->GetBinContent(_puw2016_nInt_9p2fb->FindBin(nInt)); }

TFile* puw12p9fb   = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/pileup/puWeights_12fb_63mb.root", "read");
TFile* puw12p9fbUp = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/pileup/puWeights_12fb_63mb_Up.root", "read");
TFile* puw12p9fbDn = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/pileup/puWeights_12fb_63mb_Down.root", "read");
TH1F* _puw2016_nInt_12p9fb    = (TH1F*) puw12p9fb  ->Get("puw");
TH1F* _puw2016_nInt_12p9fb_Up = (TH1F*) puw12p9fbUp->Get("puw");
TH1F* _puw2016_nInt_12p9fb_Dn = (TH1F*) puw12p9fbDn->Get("puw");

float puw2016_nInt_12p9fb(float nInt, int var = 0) { 
    TH1F* hist = _puw2016_nInt_12p9fb;
    if(var ==  1) hist = _puw2016_nInt_12p9fb_Up;
    if(var == -1) hist = _puw2016_nInt_12p9fb_Dn;
	if(var != 0 ) return hist -> GetBinContent(hist -> FindBin(nInt)) / _puw2016_nInt_12p9fb -> GetBinContent(_puw2016_nInt_12p9fb -> FindBin(nInt));
    return hist -> GetBinContent(hist -> FindBin(nInt));
}

//
//float puwMu8(int nVert) { return _puw_Mu8[nVert] * 0.001; }
//float puwMu17(int nVert) { return _puw_Mu17[nVert] * (2305428/29339.)*0.002/2.26; }




//ttH stuff

float ttH_MVAto1D_6_2lss_Marco (float kinMVA_2lss_ttbar, float kinMVA_2lss_ttV){

  return 2*((kinMVA_2lss_ttbar>=-0.2)+(kinMVA_2lss_ttbar>=0.3))+(kinMVA_2lss_ttV>=-0.1)+1;

}
float ttH_MVAto1D_3_3l_Marco (float kinMVA_3l_ttbar, float kinMVA_3l_ttV){

  if (kinMVA_3l_ttbar<0.3 && kinMVA_3l_ttV<-0.1) return 1;
  else if (kinMVA_3l_ttbar>=0.3 && kinMVA_3l_ttV>=-0.1) return 3;
  else return 2;

}

#include "ttH-multilepton/binning_2d_thresholds.h"
float ttH_MVAto1D_7_2lss_Marco (float kinMVA_2lss_ttbar, float kinMVA_2lss_ttV){

//________________
//|   |   |   | 7 |
//|   |   | 4 |___|
//| 1 | 2 |___| 6 |
//|   |   |   |___|
//|   |   | 3 | 5 |
//|___|___|___|___|
//

  if (kinMVA_2lss_ttbar<cuts_2lss_ttbar0) return 1;
  else if (kinMVA_2lss_ttbar<cuts_2lss_ttbar1) return 2;
  else if (kinMVA_2lss_ttbar<cuts_2lss_ttbar2) return 3+(kinMVA_2lss_ttV>=cuts_2lss_ttV0);
  else return 5+(kinMVA_2lss_ttV>=cuts_2lss_ttV1)+(kinMVA_2lss_ttV>=cuts_2lss_ttV2);

}
float ttH_MVAto1D_5_3l_Marco (float kinMVA_3l_ttbar, float kinMVA_3l_ttV){

  int reg = 2*((kinMVA_3l_ttbar>=cuts_3l_ttbar1)+(kinMVA_3l_ttbar>=cuts_3l_ttbar2))+(kinMVA_3l_ttV>=cuts_3l_ttV1)+1;
  if (reg==2) reg=1;
  if (reg>2) reg = reg-1;
  return reg;

}



float ttH_MVAto1D_6_flex (float kinMVA_2lss_ttbar, float kinMVA_2lss_ttV, int pdg1, int pdg2, float ttVcut, float ttcut1, float ttcut2){

  return 2*((kinMVA_2lss_ttbar>=ttcut1)+(kinMVA_2lss_ttbar>=ttcut2)) + (kinMVA_2lss_ttV>=ttVcut)+1;

}


int ttH_catIndex_2lss(int nTauTight, int LepGood1_pdgId, int LepGood2_pdgId, int LepGood1_charge, int nBJetMedium25){

//2lss_1tau
//2lss_ee_0tau_neg
//2lss_ee_0tau_pos
//2lss_em_0tau_bl_neg
//2lss_em_0tau_bl_pos
//2lss_em_0tau_bt_neg
//2lss_em_0tau_bt_pos
//2lss_mm_0tau_bl_neg
//2lss_mm_0tau_bl_pos
//2lss_mm_0tau_bt_neg
//2lss_mm_0tau_bt_pos
   
  if (nTauTight>=1) return 1;
  if (nTauTight==0 && abs(LepGood1_pdgId)==11 && abs(LepGood2_pdgId)==11 && LepGood1_charge<0) return 2;
  if (nTauTight==0 && abs(LepGood1_pdgId)==11 && abs(LepGood2_pdgId)==11 && LepGood1_charge>0) return 3;
  if (nTauTight==0 && (abs(LepGood1_pdgId) != abs(LepGood2_pdgId)) && LepGood1_charge<0 && nBJetMedium25 < 2) return 4;
  if (nTauTight==0 && (abs(LepGood1_pdgId) != abs(LepGood2_pdgId)) && LepGood1_charge>0 && nBJetMedium25 < 2) return 5;
  if (nTauTight==0 && (abs(LepGood1_pdgId) != abs(LepGood2_pdgId)) && LepGood1_charge<0 && nBJetMedium25 >= 2) return 6;
  if (nTauTight==0 && (abs(LepGood1_pdgId) != abs(LepGood2_pdgId)) && LepGood1_charge>0 && nBJetMedium25 >= 2) return 7;
  if (nTauTight==0 && abs(LepGood1_pdgId)==13 && abs(LepGood2_pdgId)==13 && LepGood1_charge<0 && nBJetMedium25 < 2) return 8;
  if (nTauTight==0 && abs(LepGood1_pdgId)==13 && abs(LepGood2_pdgId)==13 && LepGood1_charge>0 && nBJetMedium25 < 2) return 9;
  if (nTauTight==0 && abs(LepGood1_pdgId)==13 && abs(LepGood2_pdgId)==13 && LepGood1_charge<0 && nBJetMedium25 >= 2) return 10;
  if (nTauTight==0 && abs(LepGood1_pdgId)==13 && abs(LepGood2_pdgId)==13 && LepGood1_charge>0 && nBJetMedium25 >= 2) return 11;

 return -1;

}

int ttH_catIndex_3l(int LepGood1_charge, int LepGood2_charge, int LepGood3_charge, int nBJetMedium25){

//3l_bl_neg
//3l_bl_pos
//3l_bt_neg
//3l_bt_pos

  if ((LepGood1_charge+LepGood2_charge+LepGood3_charge)<0 && nBJetMedium25 < 2) return 12;
  if ((LepGood1_charge+LepGood2_charge+LepGood3_charge)>0 && nBJetMedium25 < 2) return 13;
  if ((LepGood1_charge+LepGood2_charge+LepGood3_charge)<0 && nBJetMedium25 >= 2) return 14;
  if ((LepGood1_charge+LepGood2_charge+LepGood3_charge)>0 && nBJetMedium25 >= 2) return 15;

 return -1;

}



TFile *_file_recoToLoose_leptonSF_mu1_b = NULL;
TFile *_file_recoToLoose_leptonSF_mu1_e = NULL;
TFile *_file_recoToLoose_leptonSF_mu2 = NULL;
TFile *_file_recoToLoose_leptonSF_mu3 = NULL;
TGraphAsymmErrors *_histo_recoToLoose_leptonSF_mu1_b = NULL;
TGraphAsymmErrors *_histo_recoToLoose_leptonSF_mu1_e = NULL;
TH2F *_histo_recoToLoose_leptonSF_mu2 = NULL;
TGraphAsymmErrors *_histo_recoToLoose_leptonSF_mu3 = NULL;
TFile *_file_recoToLoose_leptonSF_el = NULL;
TH2F *_histo_recoToLoose_leptonSF_el1 = NULL;
TH2F *_histo_recoToLoose_leptonSF_el2 = NULL;
TH2F *_histo_recoToLoose_leptonSF_el3 = NULL;
TFile *_file_recoToLoose_leptonSF_gsf = NULL;
TH2F *_histo_recoToLoose_leptonSF_gsf = NULL;

float _get_recoToLoose_leptonSF_ttH(int pdgid, float pt, float eta, int nlep, float var){

  if (var!=0 && abs(pdgid)!=11) assert(0); // NOT IMPLEMENTED

  if (!_histo_recoToLoose_leptonSF_mu1_b) {
    _file_recoToLoose_leptonSF_mu1_b = new TFile("../../data/leptonSF/mu_ttH_presel_barrel.root","read");
    _file_recoToLoose_leptonSF_mu1_e = new TFile("../../data/leptonSF/mu_ttH_presel_endcap.root","read");
    _file_recoToLoose_leptonSF_mu2 = new TFile("../../data/leptonSF/MuonID_Z_RunBCD_prompt80X_7p65_looseID.root","read");
    _file_recoToLoose_leptonSF_mu3 = new TFile("../../data/leptonSF/ratios_HIP_trkEff.root","read");
    _histo_recoToLoose_leptonSF_mu1_b = (TGraphAsymmErrors*)(_file_recoToLoose_leptonSF_mu1_b->Get("ratio"));
    _histo_recoToLoose_leptonSF_mu1_e = (TGraphAsymmErrors*)(_file_recoToLoose_leptonSF_mu1_e->Get("ratio"));
    _histo_recoToLoose_leptonSF_mu2 = (TH2F*)(_file_recoToLoose_leptonSF_mu2->Get("pt_abseta_ratio_MC_NUM_LooseID_DEN_genTracks_PAR_pt_spliteta_bin1"));
    _histo_recoToLoose_leptonSF_mu3 = (TGraphAsymmErrors*)(_file_recoToLoose_leptonSF_mu3->Get("ratio_eta"));
  }
  if (!_histo_recoToLoose_leptonSF_el1) {
    _file_recoToLoose_leptonSF_el = new TFile("../../data/leptonSF/el_scaleFactors_20160724.root","read");
    _histo_recoToLoose_leptonSF_el1 = (TH2F*)(_file_recoToLoose_leptonSF_el->Get("GsfElectronToFOID2D"));
    _histo_recoToLoose_leptonSF_el2 = (TH2F*)(_file_recoToLoose_leptonSF_el->Get("MVAVLooseElectronToMini4"));
    _histo_recoToLoose_leptonSF_el3 = (TH2F*)(_file_recoToLoose_leptonSF_el->Get("MVAVLooseElectronToConvIHit1"));
  }
  if (!_histo_recoToLoose_leptonSF_gsf) {
    _file_recoToLoose_leptonSF_gsf = new TFile("../../data/leptonSF/el_scaleFactors_gsf.root","read");
    _histo_recoToLoose_leptonSF_gsf = (TH2F*)(_file_recoToLoose_leptonSF_gsf->Get("EGamma_SF2D"));
  }

  if (abs(pdgid)==13){

    TGraphAsymmErrors *hist1 = (fabs(eta)<1.2) ? _histo_recoToLoose_leptonSF_mu1_b : _histo_recoToLoose_leptonSF_mu1_e;
    float pt1 = std::max(float(hist1->GetXaxis()->GetXmin()+1e-5), std::min(float(hist1->GetXaxis()->GetXmax()-1e-5), pt));
    float out = hist1->Eval(pt1);

    TH2F *hist = _histo_recoToLoose_leptonSF_mu2;
    int ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
    int etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
    out *= hist->GetBinContent(ptbin,etabin);

    hist1 = _histo_recoToLoose_leptonSF_mu3;
    float eta1 = std::max(float(hist1->GetXaxis()->GetXmin()+1e-5), std::min(float(hist1->GetXaxis()->GetXmax()-1e-5), eta));
    out *= hist1->Eval(eta1);

    return out;

  }
  if (abs(pdgid)==11){
    TH2F *hist = _histo_recoToLoose_leptonSF_el1;
    int ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
    int etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
    float out = hist->GetBinContent(ptbin,etabin)+var*hist->GetBinError(ptbin,etabin);
    hist = _histo_recoToLoose_leptonSF_el2;
    ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
    etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
    out *= hist->GetBinContent(ptbin,etabin)+var*hist->GetBinError(ptbin,etabin);
    hist = _histo_recoToLoose_leptonSF_el3;
    ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
    etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
    out *= hist->GetBinContent(ptbin,etabin)+var*hist->GetBinError(ptbin,etabin);

    hist = _histo_recoToLoose_leptonSF_gsf;
    etabin = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(eta))); // careful, different convention
    ptbin  = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(pt)));
    out *= hist->GetBinContent(etabin,ptbin);

    return out;
  }

  assert(0);
  return -999;

}

TFile *_file_looseToTight_leptonSF_mu_2lss = NULL;
TH2F *_histo_looseToTight_leptonSF_mu_2lss = NULL;
TFile *_file_looseToTight_leptonSF_el_2lss = NULL;
TH2F *_histo_looseToTight_leptonSF_el_2lss = NULL;
TFile *_file_looseToTight_leptonSF_mu_3l = NULL;
TH2F *_histo_looseToTight_leptonSF_mu_3l = NULL;
TFile *_file_looseToTight_leptonSF_el_3l = NULL;
TH2F *_histo_looseToTight_leptonSF_el_3l = NULL;

float _get_looseToTight_leptonSF_ttH(int pdgid, float pt, float eta, int nlep, float var){

  if (var!=0) assert(0); // NOT IMPLEMENTED

  if (!_histo_looseToTight_leptonSF_mu_2lss) {
    _file_looseToTight_leptonSF_mu_2lss = new TFile("../../data/lepMVAEffSF_m_2lss.root","read");
    _histo_looseToTight_leptonSF_mu_2lss = (TH2F*)(_file_looseToTight_leptonSF_mu_2lss->Get("sf"));
  }
  if (!_histo_looseToTight_leptonSF_el_2lss) {
    _file_looseToTight_leptonSF_el_2lss = new TFile("../../data/lepMVAEffSF_e_2lss.root","read");
    _histo_looseToTight_leptonSF_el_2lss = (TH2F*)(_file_looseToTight_leptonSF_el_2lss->Get("sf"));
  }
  if (!_histo_looseToTight_leptonSF_mu_3l) {
    _file_looseToTight_leptonSF_mu_3l = new TFile("../../data/lepMVAEffSF_m_3l.root","read");
    _histo_looseToTight_leptonSF_mu_3l = (TH2F*)(_file_looseToTight_leptonSF_mu_3l->Get("sf"));
  }
  if (!_histo_looseToTight_leptonSF_el_3l) {
    _file_looseToTight_leptonSF_el_3l = new TFile("../../data/lepMVAEffSF_e_3l.root","read");
    _histo_looseToTight_leptonSF_el_3l = (TH2F*)(_file_looseToTight_leptonSF_el_3l->Get("sf"));
  }

  TH2F *hist = 0;
  if (abs(pdgid)==13) hist = (nlep>2) ? _histo_looseToTight_leptonSF_mu_3l : _histo_looseToTight_leptonSF_mu_2lss;
  else if (abs(pdgid)==11) hist = (nlep>2) ? _histo_looseToTight_leptonSF_el_3l : _histo_looseToTight_leptonSF_el_2lss;
  assert(hist);
  int ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
  int etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
  return hist->GetBinContent(ptbin,etabin);

}

float leptonSF_ttH(int pdgid, float pt, float eta, int nlep, float var=0){

  float recoToLoose = _get_recoToLoose_leptonSF_ttH(pdgid,pt,eta,nlep,var);
  float looseToTight = _get_looseToTight_leptonSF_ttH(pdgid,pt,eta,nlep,var);
  float res = recoToLoose*looseToTight;
  assert (res>0);
  return res;

}

TFile *file_triggerSF_ttH = NULL;
TH2Poly* t2poly_triggerSF_ttH_mm = NULL;
TH2Poly* t2poly_triggerSF_ttH_ee = NULL;
TH2Poly* t2poly_triggerSF_ttH_em = NULL;
TH2Poly* t2poly_triggerSF_ttH_3l = NULL;

float triggerSF_ttH(int pdgid1, float pt1, int pdgid2, float pt2, int nlep, float var=0){
  if (!file_triggerSF_ttH) {
    file_triggerSF_ttH = new TFile("../../data/triggerSF/trig_eff_map_v4.root");
    t2poly_triggerSF_ttH_mm = (TH2Poly*)(file_triggerSF_ttH->Get("SSuu2DPt_effic"));
    t2poly_triggerSF_ttH_ee = (TH2Poly*)(file_triggerSF_ttH->Get("SSee2DPt__effic"));
    t2poly_triggerSF_ttH_em = (TH2Poly*)(file_triggerSF_ttH->Get("SSeu2DPt_effic"));
    t2poly_triggerSF_ttH_3l = (TH2Poly*)(file_triggerSF_ttH->Get("__3l2DPt_effic"));
    if (!(t2poly_triggerSF_ttH_mm && t2poly_triggerSF_ttH_ee && t2poly_triggerSF_ttH_em && t2poly_triggerSF_ttH_3l)) {
	std::cout << "Impossible to load trigger scale factors!" << std::endl;
	file_triggerSF_ttH->ls();
	file_triggerSF_ttH = NULL;
      }
  }
  TH2Poly* hist = NULL;
  if (nlep==2){
    if (abs(pdgid1)==13 && abs(pdgid2)==13) hist = t2poly_triggerSF_ttH_mm;
    else if (abs(pdgid1)==11 && abs(pdgid2)==11) hist = t2poly_triggerSF_ttH_ee;
    else hist = t2poly_triggerSF_ttH_em;
  }
  else if (nlep==3) hist = t2poly_triggerSF_ttH_3l;
  else std::cout << "Wrong options to trigger scale factors" << std::endl;
  pt1 = std::max(float(hist->GetXaxis()->GetXmin()+1e-5), std::min(float(hist->GetXaxis()->GetXmax()-1e-5), pt1));
  pt2 = std::max(float(hist->GetYaxis()->GetXmin()+1e-5), std::min(float(hist->GetYaxis()->GetXmax()-1e-5), pt2));
  int bin = hist->FindBin(pt1,pt2);
  float eff = hist->GetBinContent(bin) + var * hist->GetBinError(bin);

  if (nlep>2) return eff;
  int cat = (abs(pdgid1)==11) + (abs(pdgid2)==11);
  if (cat==2) return eff*1.02;
  else if (cat==1) return eff*1.02;
  else return eff*1.01;


}

float mass_3_cheap(float pt1, float eta1, float pt2, float eta2, float phi2, float pt3, float eta3, float phi3) {
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > PtEtaPhiMVector;
    PtEtaPhiMVector p41(pt1,eta1,0,   0.0);
    PtEtaPhiMVector p42(pt2,eta2,phi2,0.0);
    PtEtaPhiMVector p43(pt3,eta3,phi3,0.0);
    return (p41+p42+p43).M();
}




//#include "TGraphAsymmErrors.h"
//TFile *_file_reco_leptonSF_mu = NULL;
//TFile *_file_recoToMedium_leptonSF_mu = NULL;
//TFile *_file_MediumToMVA_leptonSF_mu = NULL;
//TFile *_file_recoToMVA_leptonSF_el = NULL;
//TFile *_file_reco_leptonSF_el = NULL;
//
//TGraphAsymmErrors *_histo_reco_leptonSF_mu = NULL;
//TH2F *_histo_recoToMedium_leptonSF_mu = NULL;
//TH2F *_histo_MediumToMVA_leptonSF_mu = NULL;
//TH2F *_histo_recoToMVA_leptonSF_el = NULL;
//TH2F *_histo_reco_leptonSF_el = NULL;
//
//float getLeptonSF_mu_Unc(float pt, int var) {
//  if (pt<20) 
//    return var*TMath::Sqrt(0.03*0.03+0.01*0.01+0.01*0.01);
//  else 
//    return var*TMath::Sqrt(0.02*0.02+0.01*0.01);  
//}
//float leptonSF_2lss_ewk(int pdgid, float pt, float eta, int var=0){
//  
//  if (!_histo_reco_leptonSF_mu) {
//     _file_reco_leptonSF_mu = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/SF2016_muon_trackingEff.root", "data");
//     _file_recoToMedium_leptonSF_mu = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/SF2016_muon_mediumId.root", "read");
//     _file_MediumToMVA_leptonSF_mu = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/SF2016_muon_lepMVAveryTight.root", "read");
//     _histo_reco_leptonSF_mu = (TGraphAsymmErrors*)(_file_reco_leptonSF_mu->Get("ratio_eta"));
//     _histo_recoToMedium_leptonSF_mu = (TH2F*)(_file_recoToMedium_leptonSF_mu->Get("pt_abseta_PLOT_pair_probeMultiplicity_bin0"));
//     _histo_MediumToMVA_leptonSF_mu = (TH2F*)(_file_MediumToMVA_leptonSF_mu->Get("pt_abseta_PLOT_pair_probeMultiplicity_bin0_&_tag_combRelIsoPF04dBeta_bin0_&_tag_pt_bin0_&_mvaPreSel_pass"));
//   }
//   if (!_histo_recoToMVA_leptonSF_el) {
//     _file_recoToMVA_leptonSF_el = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/SF2016_electron_full.root", "read");
//     _histo_recoToMVA_leptonSF_el = (TH2F*)(_file_recoToMVA_leptonSF_el->Get("GsfElectronToLeptonMvaVTIDEmuTightIP2DSIP3D8miniIso04"));
//     
//     _file_reco_leptonSF_el = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/SF2016_electron_trackingEff.root", "read");
//     _histo_reco_leptonSF_el = (TH2F*) (_file_reco_leptonSF_el->Get("EGamma_SF2D"));
//   }
//   float out = 0.;
//   if (abs(pdgid)==13){
//     out = _histo_reco_leptonSF_mu->Eval(eta);
//     TH2F *hist = _histo_recoToMedium_leptonSF_mu;
//     int ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
//     int etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
//     out *= hist->GetBinContent(ptbin,etabin);
//     hist = _histo_MediumToMVA_leptonSF_mu;
//     ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
//     etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
//     out *=hist->GetBinContent(ptbin,etabin);
//     return out + out*getLeptonSF_mu_Unc(pt,var);
//   }
//   float err = 0.;
//   if (abs(pdgid)==11){
//     TH2F *hist = _histo_recoToMVA_leptonSF_el;
//     int ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
//     int etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
//     out = hist->GetBinContent(ptbin,etabin);
//     err = hist->GetBinError(ptbin,etabin)*hist->GetBinError(ptbin,etabin);
//     hist = _histo_reco_leptonSF_el;
//     ptbin  = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(pt)));
//     etabin = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(eta)));
//     out *= hist->GetBinContent(etabin,ptbin);
//     err += hist->GetBinError(etabin,ptbin)*hist->GetBinError(etabin,ptbin);
//     err = TMath::Sqrt(err);
//     return out + out*err*var;
//   }
//   //cout << "[ERROR]!!!! SF UnKNOWN!!! PLEASE CHECK" << endl;
//   return 1.;
// }
//
//
//
//TFile* trigSF = NULL; 
//TH2F* _trigSF_2l_m = NULL;
//TH2F* _trigSF_2l_e = NULL;
//
//float triggerSF_2lss_ewk(float pt1, float pt2, int pdg2){
//  // Lesya's mail:
//  // - split for trailing ele or trailing mu
//  // - 3l: subleading vs trailing lepton pt (1l + 2l triggers)
//  // - 2l: leading light lepton vs subleading light lepton ==> good for both 2l+tau and 2lSS cases (1l + 2l triggers)
//  // - l+tautau: use flat 86% everywhere; pt_e > 35 GeV; pt_mu > 25 GeV (1l + l/tau triggers) 
//  if (!_trigSF_2l_m) { 
//    trigSF = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/triggerSF/EWKino_9p2_triggerSF.root", "read");
//    _trigSF_2l_m = (TH2F*) trigSF->Get("eff_2l_mu" );
//    _trigSF_2l_e = (TH2F*) trigSF->Get("eff_2l_ele");
//  }
//  // 2l
//  TH2F* hist = (pdg2 == 13)?_trigSF_2l_m:_trigSF_2l_e;
//  int xbin = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt1)));
//  int ybin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(pt2)));
//  //  cout << pt1 << " " << pt2 << " " << xbin << " " << ybin << " " << hist->GetBinContent(xbin,ybin) << endl;
//  return hist->GetBinContent(xbin,ybin);
//}
//
//#include "TGraphAsymmErrors.h"
//TFile *_file_reco_leptonSF_mu = NULL;
//TFile *_file_recoToMedium_leptonSF_mu = NULL;
//TFile *_file_MediumToMVA_leptonSF_mu = NULL;
//TFile *_file_recoToMVA_leptonSF_el = NULL;
//TFile *_file_reco_leptonSF_el = NULL;
//
//TGraphAsymmErrors *_histo_reco_leptonSF_mu = NULL;
//TH2F *_histo_recoToMedium_leptonSF_mu = NULL;
//TH2F *_histo_MediumToMVA_leptonSF_mu = NULL;
//TH2F *_histo_recoToMVA_leptonSF_el = NULL;
//TH2F *_histo_reco_leptonSF_el = NULL;
//
//float getLeptonSF_mu_Unc(float pt, int var) {
//  if (pt<20) 
//    return var*TMath::Sqrt(0.03*0.03+0.01*0.01+0.01*0.01);
//  else 
//    return var*TMath::Sqrt(0.02*0.02+0.01*0.01);  
//}
//float leptonSF_2lss_ewk(int pdgid, float pt, float eta, int var=0){
//  
//  if (!_histo_reco_leptonSF_mu) {
//     _file_reco_leptonSF_mu = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/sf_mu_trk_susy_ICHEP.root","read");
//     _file_recoToMedium_leptonSF_mu = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/sf_mu_Medium_susy_ICHEP.root","read");
//     _file_MediumToMVA_leptonSF_mu = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/sf_mu_MVAVT_susy_ICHEP.root","read");
//     _histo_reco_leptonSF_mu = (TGraphAsymmErrors*)(_file_reco_leptonSF_mu->Get("ratio_eta"));
//     _histo_recoToMedium_leptonSF_mu = (TH2F*)(_file_recoToMedium_leptonSF_mu->Get("pt_abseta_PLOT_pair_probeMultiplicity_bin0"));
//     _histo_MediumToMVA_leptonSF_mu = (TH2F*)(_file_MediumToMVA_leptonSF_mu->Get("pt_abseta_PLOT_pair_probeMultiplicity_bin0_&_tag_combRelIsoPF04dBeta_bin0_&_tag_pt_bin0_&_mvaPreSel_pass"));
//   }
//   if (!_histo_recoToMVA_leptonSF_el) {
//     _file_recoToMVA_leptonSF_el = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/sf_el_susy_ICHEP.root","read");
//     _histo_recoToMVA_leptonSF_el = (TH2F*)(_file_recoToMVA_leptonSF_el->Get("GsfElectronToLeptonMvaVTIDEmuTightIP2DSIP3D8miniIso04"));
//     
//     _file_reco_leptonSF_el = new TFile(CMSSW_BASE+"/src/CMGTools/TTHAnalysis/data/leptonSF/sf_el_trk_susy_ICHEP.root","read");
//     _histo_reco_leptonSF_el = (TH2F*) (_file_reco_leptonSF_el->Get("EGamma_SF2D"));
//   }
//   float out = 0.;
//   if (abs(pdgid)==13){
//     out = _histo_reco_leptonSF_mu->Eval(eta);
//     TH2F *hist = _histo_recoToMedium_leptonSF_mu;
//     int ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
//     int etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
//     out *= hist->GetBinContent(ptbin,etabin);
//     hist = _histo_MediumToMVA_leptonSF_mu;
//     ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
//     etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
//     out *=hist->GetBinContent(ptbin,etabin);
//     return out + out*getLeptonSF_mu_Unc(pt,var);
//   }
//   float err = 0.;
//   if (abs(pdgid)==11){
//     TH2F *hist = _histo_recoToMVA_leptonSF_el;
//     int ptbin  = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(pt)));
//     int etabin = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(fabs(eta))));
//     out = hist->GetBinContent(ptbin,etabin);
//     err = hist->GetBinError(ptbin,etabin)*hist->GetBinError(ptbin,etabin);
//     hist = _histo_reco_leptonSF_el;
//     ptbin  = std::max(1, std::min(hist->GetNbinsY(), hist->GetYaxis()->FindBin(pt)));
//     etabin = std::max(1, std::min(hist->GetNbinsX(), hist->GetXaxis()->FindBin(eta)));
//     out *= hist->GetBinContent(etabin,ptbin);
//     err += hist->GetBinError(etabin,ptbin)*hist->GetBinError(etabin,ptbin);
//     err = TMath::Sqrt(err);
//     return out + out*err*var;
//   }
//   cout << "[ERROR]!!!! SF UnKNOWN!!! PLEASE CHECK" << endl;
//   return 1.;
// }


float metmm_pt(int pdg1, float pt1, float phi1, int pdg2, float pt2, float phi2, float metpt, float metphi) {
  if (abs(pdg1)==13 && abs(pdg2)==13) return pt_3(pt1,phi1,pt2,phi2,metpt,metphi);
  else if (abs(pdg1)==13 && !(abs(pdg2)==13)) return pt_2(pt1,phi1,metpt,metphi);
  else if (!(abs(pdg1)==13) && abs(pdg2)==13) return pt_2(pt2,phi2,metpt,metphi);
  else if (!(abs(pdg1)==13) && !(abs(pdg2)==13)) return metpt;
  else return -99;
}


void functions() {}





