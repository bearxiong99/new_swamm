#include <string.h>

#include "typedef.h"
#include "mcudef.h"
#include "codeUtility.h"


/** Manufacturer 3 Letter Code
  *
  * http://www.dlms.com/organization/flagmanufacturesids/index.html
  *
  * Date: 30th April 2013

  ABB ABB AB, P.O. Box 1005, SE-61129 Nyköping, Nyköping,Sweden
  ABN ABN Braun AG, Platenstraße 59, 90441 Nürnberg, Germany
  ACA Acean, Zi de la Liane, BP 439, 62206 Boulogne Sur Mer Cedex, FRANCE
  ACE Actaris, France. (Electricity)
  ACG Actaris, France. (Gas)
  ACW Actaris, France. (Water and Heat)
  ADD "ADD-Production" S.R.L., 36, Dragomirna str., MD-2008, Chisinau, Republic of Moldova
  ADX ADD-Production S.R.L., 36, Dragomirna str., MD-2008, Chisinau, Republic of Moldova
  ADN Aidon Oy, 40101 Jyvaskyla, Finland
  ADU Adunos GmbH, Am Schlangengraben 16, D-13597 Berlin, Germany
  AEC Advance Electronics Company, Riyadh, Saudi Arabia
  AEE Atlas Electronics, 17530 Surdulica, Serbia and Montenegro
  AEG AEG
  AEL Kohler, Turkey
  AEM S.C. AEM S.A. Romania
  AER Aerzener Maschinenfabrik GmbH, Reherweg 28, 31855 Aerzen, Germany
  AFX Alflex Products, Zoetermeer, Holland
  AGE AccessGate AB, Rissneleden 144, 174 57 Sundbyberg, Sweden
  ALF Alfatech - Elektromed Elektronik, Ankara, Turkey
  ALG Algodue Elettronica srl, Via Passerina, 3/A, Fontaneto D\'Agogna, Italy
  ALT Amplitec GmbH, Gootkoppel 28, Reinfeld, Germany
  AMB Amber wireless GmbH, Hawstra e 2a, 54290 Trier, Germany
  AME AVON METERS PRIVATE LIMITED, D-15/16/17, INDUSTRIAL FOCAL POINT, DERABASSI,PUNJAB-140507INDIA
  AMH AMiHo Ltd, 1010 Cambourne Business Park, Cambourne, Cambridge, CB1 9AY, UK
  AMI AMI Tech(I) Pvt. Ltd, #205&206,NSIC-EMDBP, Kamalanagar, ECIL PO, Hyderabad- 500062, India
  AML Eon Electric Ltd., C-124, Hosierry Complex, Noida Phase II, NOIDA, INDIA
  AMP Ampy Automation Digilog Ltd
  AMS Zhejiang Chaoyi Electronic Technology Co. Ltd., Zhejiang, China.
  AMT Aquametro
  AMX APATOR METRIX SA, Piaskowa 3, 83-110 Tczew, Poland
  ANA Anacle Systems PTE LTD, 1A International Business Park #05-02, Singapore
  AND ANDIS sro, Bratislava, Slovakia
  APA APATOR SA (Electricity), Zolkiewskiego 21/29, 87-100, Torun,Poland
  APR Apronecs Ltd, Gabrovo, Bulgaria
  APS Apsis Kontrol Sistemleri, Turkey
  APT Apator SA (Gas, water and heat), ó kiewskiego 21/29, Toru , Poland
  APX Amplex A/S, Aarhus C, Denmark
  AQM Aquametro AG, Ringstrasse 75, Therwil, Switzerland
  AQT AQUATHERM P.P.H, Kujawinski, Lomianki, Poland
  ARC Arcelik AS., Istanbul, Turkey
  ARF ADEUNIS_RF, 283 rue Louis NEEL, CROLLES 38920, France
  ARM arivus metering GmbH, Mielestr. 2, 14542 Werder (Havel), Germany
  ARS ADD - Bulgaria Ltd, Bul. \"6 septemvri\" 252 Et.7, Plovdiv, Bulgaria
  ART Electrotécnica Arteche Smart Grid, Derio Bidea 28, Zabalondo Industrialdea, 48100 Mungia, Bizkaia. Spain
  ASR Erelsan Elektrik ve Elektronik, Malzeme, Istanbul, Turkey
  AST ASTINCO Inc., 114 Anderson Ave. Suite 7A, ON, L6E1A5, Markham, Canada
  ATF AKTIF Otomasyon ve GS ve Tic, Turkey
  ATI ANALOGICS TECH INDIA LIMITED, Plot No.9/10, Road No.6, Nacharam Industrial Estate, HYDERABAD, INDIA
  ATL Atlas Elektronik, ANKARA, Turkey
  ATM Atmel, Torre C2, Polígono Puerta norte, A-23, 50820, (Zaragoza) Spain
  ATS Atlas Sayaç Sanayi A. ., Erciyes Teknopark 4.Bina Talas, Kayseri, Turkey
  AUX Ningbo Sanxing Electric Co., Ltd., No.1166, Mingguang North Rd. Jiangshan Town, Ningbo, China
  AXI UAB "Axis Industries", LT-47190, Lithuania
  AZE AZEL Electronics, B. Ankara, Turkey
  BAM Bachmann GmbH & Co KG, Ernsthaldenstr, 33 70565, Stuttgart, Germany
  BAR Baer Industrie-Elektronik GmbH, Fuerth, Germany
  BAS BASIC INTELLIGENCE TECHNOLOGY CO.,LTD., 1st Floor, No.1 NanLi Rd. PanYu District, GuangZhou, GuangDong, China.
  BBS BBS Electronics, Singapore
  BCE ShenZhen B.C Electronic CO.Ltd, 4F, Strength Building,GaoXin Ave.1.s, South, Hi-technology industry Zone, ShenZhen, China
  BEE Bentec Electricals & Electronics Pvt Ltd, 150,Upen Banerjee Road,Kolkata, Kolkata, India
  BEF BEFEGA GmbH, Reichenbacher Str. 22, Schwabach, Germany
  BEG begcomm Communication AB, Brunnehagen 109, GöteborgSweden
  BER Bernina Electronic AG
  BHG Brunata A/S, DK-2730 Herlev, Denmark
  BKB Boendekomfort AB, Box 37, 260 40 Viken, SWEDEN
  BKT Bekto Precisa, Ibrahima Popovi a bb., Gora de, Bosnia and Herzegovina
  BKO Beko Elektronik AS., Istanbul, Turkey
  BLU Bluering, Brescia, Italy
  BME Beifeng GmbH, 60599 Frankfurt am Main, Germany
  BMI Badger Meter Inc., 6116 E 15th St., Tulsa, USA
  BMP BMETERS Polska Sp.z.o.o., Glowna 60, Psary, Poland
  BMT BMETERS srl, Via Friuli, 3, 33050, Gonars (UD), ITALY
  BRA Brandes GmbH, D-23701 Eutin, Germany
  BSC Sanayeh Sanjesh Energy Behineh Sazan Toos, Toos Industrial Estate,Mashhad,Iran
  BSE Basari Elektronik A.S. Turkey
  BSM Bluestar Electrical Meter Research Institute, Nanjing, China
  BSP Byucksan Power Co. Ltd., 6th Fl. New Hosung Bldg. Yoido-Dong, Youngdeungpo-Gu, Seoul, Korea.
  BST BESTAS Elektronik Optik, Turkey
  BSX BS-Messtechnik UG, Kassel, Germany
  BTL BIT-LAB
  BTR RIA-BTR Produktions-GmbH, D-78176 Blumberg
  BTS Basari Teknolojik Sistemler AS, Ankara, Turkey
  BUR Bopp und Reuther Messtechnik GmbH, Speyer, Germany
  BYD BYD Company Limited, BYD Road NO.3009 PingShan, ShenZhen, China
  BYL BAYLAN, Turkey
  BXC Beijing Fuxing Xiao-Cheng Electronic Technology Stock Co., Ltd, Room 503, Block D, IFEC Blog, No.87 Xisanhuan Beilu, Haidian District, Beijing, China
  BZR Gebr. Bauer GbR, 87719 Mindelheim, Germany
  CAH MAEC GROUPE CAHORS, ZI DE REGOURD BP 149, 46003 CAHORS CEDEX 9, FRANCE
  CAT Cubes And Tubes OY, Olli Kytölän tie 1, MUURAME, FINLAND
  CBI Circuit Breaker Industries, South Africa
  CDL Customised Data Ltd., 44 Allerburn Lea, Alnwick, UK
  CEB Cebyc AS, Vestre Rosten 81 / 13th Floor, Tiller, Norway
  CET Cetinkaya Aydinlatma, Istanbul, Turkey
  CGC Contor Group S.A., Romania - 310059 Arad, Calea Bodrogului nr. 2-4
  CIR Circutor, Viladecavalls/Barcelona, Spain
  CLE Shen Zhen Clou Electronics Co Ltd, Guangdong, China
  CLO Clorius Raab Karcher Energi Service A/S
  CLY Clayster, FO Petersons gata 6, 421 31 Västra Frölunda, Sweden
  CMC CMC EKOCON d.o.o., IOC ZAPOLJE I/10, LOGATEC, SLOVENIA
  CMP CM Partner Inc, Yongin, South Korea
  CMV Comverge, Inc., 5390 Triangle Parkway, Suite 300, Norcross, GA 30041, USA
  CNM COSTEL, #462-870 COSTEL Bldg., 223-39, Sangdaewon-Dong, Jungwon-Gu, Sungnam-Si, Kyunggi-Do, Korea
  COM COMMON S.A., Aleksandrowska 67/93, LODZ, POLAND
  CON Conlog
  CPL CPL CONCORDIA Soc.Coop., Via A. Grandi, 39 - 41033 Concordia s/S (MO), Italy
  CPO C3PO S.A., Alejandro Goicoechea 6, Sant Just Desvern, Spain
  CRW Xuzhou Runwu Science and Technology Development Co. Ltd., NO.5,Huijin Road, Damiao Industry Park,Economic Development Zone, Xuzhou,Jiangsu, P.R.China
  CTR Contar Electronica Industrial, Lisboa, Portugal
  CTX Contronix GmbH, Nizzastr 6, Radebeul, Germany
  CUC cuculus GmbH, Ehrenbergstrasse 11, D-98693 Ilmenau,Germany
  CUR CURRENT Group, LLC, 20420 Century Boulevard, Germantown, MD, USA
  CWI Cewe Instrument AB, Nykoping, Sweden
  CWV CMEC Electric Import & Export Co. Ltd, Beijing 100055, China
  CYE Quanzhou Chiyoung Electronics Technology Co. Ltd., #20 Hongshan Rd,Shudou community, Changtai St, Quanzhou City, Fujian 362000, China
  CYI QUANZHOU CHIYOUNG INSTRUMENT CO., LTD, #20 Hongshan Rd,Shudou community, Changtai St, Quanzhou City, Fujian, China 362000
  CYN Cynox, Weinart Engineering, Bad Zwischenahn, Germany
  CZA Contazara, Zaragoza, Spain
  CZM Cazzaniga S.p.A.
  DAE DAE Srl, Via Trieste, 4/E, Santa Lucia di Piave, Italy
  DAF Daf Enerji Sanayi ve Ticaret A.S, Atasehir Bulvari Ata Carsi Kat:4 No:52 34758 Atasehir, Istanbul, Turkey
  DAN Danubia
  DBE Decibels Electronics P Ltd, Decibels Electronics Pvt Ltd., 6-1-85/4, Saifabad, Hyderabad, AP, India
  DDE D&D Elettronica srl, Via XXV Aprile, 37, Bresso (MI), ITALY
  DEC DECODE d.o.o. Data Communications, Belgrade
  DEL DELTAMESS DWWF GmbH, Sebenter Weg 42, 23758 Oldenburg in Holstein, Germany
  DES Desi (Alarms) Ltd, Turkey
  DEV Develco Products, Olof Palmes Allé 40, 8200 Aarhus N, Denmark
  DFE Dongfang Electronics Co., Ltd., JiChang road 2#, Yantai City, Shandong Province, China
  DFS Danfoss A/S
  DGC Digicom S.p.A., Via A.Volta 39, 21010 Cardano al Campo (VA), Italy
  DGM Diehl Gas Metering GmbH, Industriestrasse 13, Ansbach, Germany
  DIE Dielen GmbH, Zeppelinstrasse 9, 47638 Straelen, Deutschland
  DJA De Jaeger Automation bvba, Molenstraat 200, B-9900 EEKLO, Belgium
  DKY Electric Power Research Institute of Guangdong Power Grid Corporation, No. 8, Shui jungang Dongfengdong Road, Guangzhou, China
  DIL DECCAN INFRATECH LIMETED, A3-4/A,Electronic Complex, Kushaiguda, HYDERABAD, INDIA
  DMC DMC International, Al Gharhoud, Dubai, UAE
  DME DIEHL Metering, Industriestrasse 13, 91522 Ansbach, Germany
  DMP DM Power Co., Ltd, #SB118 Megavalley, Gwanyang,-Dong, Anyang City, South Korea
  DNO DENO d.o.o, Zagreb, Croatia
  DNT Dr Neuhaus Telekommunikation GmbH, Hamburg, Germany
  DNV DNV KEMA, Utrechtseweg 310, Arnhem, Netherlands
  DPP DECCAN POWER PRODUCTS PVT. LTD., A3-4/A, Electronic Complex, Kushaiguda, Hyderabad-500062, INDIA
  DRT DRESSER Italia S.r.l., Via Roma, 772, Talamona (SO), Italy
  DSE Digitech Systems and Engineering Private Limited, 18 Ramamurthy Street, Nehru Nagar, Chromepet, Chennai-600044, Tamil Nadu, India
  DSE DSE energy Co., Ltd, 8F, No.531, HsinTien, Taipei, Taiwan
  DWZ Lorenz GmbH & Co. KG, Burgweg 3, 89601 Schelklingen, Germany
  DZG Deutsche Zahlergesellschaft
  EAA Electronic Afzar Azma, Iran
  EAH Endress+Hauser, 87484 Nesselwang, Germany
  EAS EAS Elektronik San. Tic. A.S., Ankara, Turkey
  ECH Echelon Corporation, 550 Meridian Avenue, San Jose, California, USA.
  ECL Electronics Corporation of India Ltd, Hyderabad, India
  ECS Herholdt Controls srl, Milan, Italy
  EDI Enel Distribuzione S.p.A, Via Ombrone, 2, Rome, Italy
  EDM EDMI Pty. Ltd.
  EEE 3E s.r.l., Via Biandrate, 24, Novara, Italy
  EEO Eppeltone Engineers, A 293/1 Okhla Industrial Area Phase 1, New Delhi, India
  EFA EFACEC Engenharia e Sistemas SA, Apartado 3078, MAIA, PORTUGAL
  EFE Engelmann Sensor GmbH, Rudolf-Diesel-Straße 24-28, 69168 Wiesloch, Germany
  EFR Europäische Funk-Rundsteuerung, Nymphenburger Strasse 20b, Munich, Germany
  EGA eGain International AB, Faktorvägen 9, Kungsbacka, Sweden
  EGM Elgama-Elektronika Ltd, Lithuania
  EHL Secure Meters Limited
  EIT EnergyICT NV, 8500 Kortrijk, Belgium
  EKA Eka Systems, Germantown, MD 20874, USA
  EKT PA KVANT J.S., Russian Federation
  ELD Elektromed Elektronik Ltd, Turkey, O.S.B. Uygurlar Cad. No:4 Sincan, Ankara, Turkey
  ELE Elster Electricity LLC, 208 Rogers Lane, Raleigh, USA
  ELG Elgas s.r.o., Pardubice, Czech Republic
  ELM Elektromed Elektronik Ltd, Turkey
  ELO ELO Sistemas Eletronicos S.A., Brazil
  ELQ ELEQ b.v., Karl-Ferdinand-Braun-Straße 1, Kerpen, Germay
  ELR Elster Metering Limited, 130 Camford Way, Luton, UK
  ELS Elster GmbH, 55252 Mainz-Kastell, Germany
  ELT ELTAKO GmbH, Hofener Straße 54, 70736 Fellbach, Germany
  ELV Elvaco AB, Kungsbacka, Sweden
  EMC Embedded Communication Systems GmbH, vom Staal-Weg 10, 4500 Solothurn, Switzerland
  EME SC. Electromagnetica SA, Bucharest, Romania
  EMH EMH metering GmbH & Co. KG (formerly EMH Elektrizitatszahler GmbH & CO KG)
  EML Emlite ltd, 10 Reynolds Business Park, Stevern Way, PE1 5EL Peterborough, UK
  EMM Email Metering, Australia
  EMO Enermet
  EMS EMS-PATVAG AG, CH-7013 Domat/Ems, Switzerland
  EMT Elster Messtechnik GmbH, Lampertheim, Germany
  EMU EMU Elektronik AG, 6432 Rickenbach SZ, Switzerland
  END ENDYS GmbH
  ENE ENERDIS, 16 rue Georges Besse SILIC44, 92182 ANTONYFRANCE
  ENG ENER-G Switch2 Ltd, The Waterfront, Salts Mill Rd, Bradford, BD17 7EZ, UK
  ENI entec innovations GmbH, Hebelstr. 1, 79379 Müllheim, Germany
  ENL ENEL d.o.o. Beograd, Belgrade, Serbia and Montenegro
  ENM ENMAS GmbH, Holzkoppelweg 23, Kiel, Germany
  ENO ennovatis GmbH, Stammheimer 10Kornwestheim, Germany
  ENP "Kiev Polytechnical Scientific Research"
  ENR Energisme
  ENS ENSO NETZ GmbH, Postfach 12 01 23, 01002 Dresden, Dresden, Deutschland
  ENT ENTES Elektronik, Istambul
  ERE Enermatics Energy (PTY) LTD, Mertech Building, Glenfield Office Park, Oberon str., Faerleglen, Pretoria, South Africa
  ERI Easun Reyrolle Limited, 389, Rasukumaki, Hulimavu, Bannerghatta Road, Bangalore-560076, India
  ERL Erelsan Elektrik ve Elektronik, Turkey
  ESE ESE Nordic AB, Slottagårdsgatan 9, Vellinge, Sweden
  ESI Monosan Monofaze Elektrik Motorlari, Turkey
  ESM Monosan Monofase Elekrik Motorlari, Turkey
  ESO Monosan Monofaze Elektrik Motorlari, Turkey
  ESS Energy Saving Systems LTD., Zroshyvalna, 15b, Kiev, Ukraine
  ESY EasyMeter GmbH
  EUE E+E Electronic, Langwiesen 7, 4209 Engerwitzdorf, Austria
  EUR Eurometers Ltd
  EVK EV KUR ELEKTRIK, Istanbul, Turkey
  EWG EWG DOO, Bulevar Svetog Cara Konstantina 80-82, Ni , 18106, Serbia
  EWT Elin Wasserwerkstechnik
  EYT Enerlyt Potsdam GmbH
  FAN Fantini Cosmi S.p.A., Via dell"Osio 6, 20090 Caleppio di Settala, Miano, Italy
  FED Federal Elektrik, Turkey
  FFD Fast Forward AG, Ruedesheimer Strasse 11, Munich, Germany
  FIM Frodexim Ltd, Sofia, Bulgaria
  FIN Finder GmbH, Hans-Böckler-Starße 44, 65468 Trebur-Astheim, Deutschland
  FLE XI'AN FLAG ELECTRONIC CO.,LTD, Flag Electronic Industry Park,No.11,Zhangba 6 Rd.(New Zone), Hi-Tech Development Zone, Xi'an, ShaanXi, PRC., China
  FIO Pietro Fiorentini, Via Rosellini,1, Milano, Italy
  FLG FLOMAG s.r.o, Brno, Czech Republic
  FLO Flonidan A/S, 8700 Horsens, Denmark
  FLS FLASH o.s, Istanbul, Turkey
  FMG Flow Meter Group, Menisstraat 5c, 7091 ZZ Dinxperlo, The Netherlands
  FML Siemens Measurements Ltd. (Formerly FML Ltd.)
  FNX Flownetix Ltd, Marlow Bottom, Bucks, UK
  FRE Frer Srl, Viale Europa, 12, Cologno Monzese (MI), Italy
  FSP Finmek Space S.p.A., I-34012 Trieste
  FTL Tritschler GmbH, Schönaustr. 10+12, Laufenburg, Deutschland
  FUS Fuccesso, 98 Yingchundong, Taizhou, China
  FUT first:utility, Tachbrook Park, Warwick, UK
  FWS FW Systeme GmBH, Ehnkenweg 11, 26125 Oldenburg, Germany
  GAV Carlo Gavazzi Controls S.p.A, Via Safforze 8 C.A.P. 32100, Belluno, Italy
  GBJ Grundfoss A/S
  GCE Genergica, Caracas, Venezuela
  GEC GEC Meters Ltd.
  GEE GE Energy, Lauder House, Almondvale Business Park, Livingston, UK
  GEL Industrial Technology Research Institute, Rm. 809, Blg.51, No. 195, Sec. 4, Chung Hsing Rd., Chutung, Hsinchu, Taiwan
  GEN Goerlitz AG, Germany
  GEO Green Energy Options Limited, 3 St. Mary"s Court, Main Street Hardwick, Camridge, England, CB23 7QS
  GET Genus Electrotech Ltd., Survey No-43,Galpadar Road, Taluka anjar, District-kutch, gandhidham-370110 Gujrat, Taluka anjar,India
  GFM GE Fuji Meter Co.,Ltd., Horigane Karasugawa 2191, Azumino-City Nagano, Japan
  GIL Genus Innovations Limited, SPL-2B, RIICO Industrial Area, Sitapura, Jaipur, India
  GIN Gineers Ltd, 1756 Sofia, Bulgaria
  GMC GMC-I Messtechnik GmbH, Südwestpark 15, D-90449 Germany
  GME Global Metering Electronics, Amsterdam, Netherlands
  GMM Gamma International Egypt, Abour, St 130, industrial area, Cairo, Egypt
  GMT GMT GmbH, Odenwaldstraße 19, 64521 Groß-Gerau, Germany
  GOE Genus Overseas Electronics Ltd, Jaipur, India
  GRE GE2, Green Energy Electronics, R. Fonte Caspolina, N.6,2.C, 2774-521, PACO DE ARCOS, Portugal.
  GRI Grinpal Energy Management, 50 Oak Avenue, Pretoria, South Africa
  GSP Ingenieurbuero Gasperowicz
  GST Shenzhen Golden Square Technology Co.,Ltd, Zone C&D,5/F,Block A3,Shenzhen Digital Technology Park,Hi-Tech South 7 Rd.,Nanshan ,Shenzhen,Guangdong, China
  GTE GREATech GmbH, Lindenstrasse 66a, 45478 Muelheim an der Ruhr, Germany
  GUH ShenZhen GuangNing Industrial CO.,Ltd, Room 802, 8th Floor, ShenZhen Software Building ,NanShan, District,ShenZhen ,China
  GWF Gas- u. Wassermesserfabrik Luzern
  HAG Hager Electro GmbH, 66131 Saarbruecken, Germany
  HCE Hsiang Cheng Electric Corp, Hsin-Tien City, Taipei, R.o.China
  HEG Hamburger Elektronik Gesellschaft
  HEI Hydro-Eco-Invest SP. Z O.O., Gliwice, Poland
  HEL Heliowatt
  HER Hermes Systems, Australia
  HFR SAERI HEAT METERING TECHNOLOGY CO.,LTD, WANLIAN ROAD 1,SHENHE DISTRICT SHENYANG CHINA
  HIE Shenzhen Holley South Electronics Technology Co., Ltd., 7/F, No.2 Jianxing Building, Chaguang Industrial Zone, Nanshan District, Shenzhen, China
  HLY Holley Metering Ltd
  HMI HMI Energy Co., Ltd, No.38, Alley 175, Lane 75, Sec3, Kongning Rd., Neihu, Taipe, Taiwan
  HMS Hermes Systems, Australia
  HMU Hugo Müller GmbH & Co KG, Sturmbühlstraße 145-149, 78054 VS-Schwenningen, Germany
  HND Shenzhen Haoningda Meters Co., Ltd., 6/F, Huake Mansion, East Science Park, Qiaoxiang Rd, Nanshan District, Shenzhen, China
  HOE HOENTZSCH GMBH, Gottlieb-Daimler-Str.37, 71334 Waiblingen, Germany
  HOL Holosys d.o.o, Matije Gupca 7, 49243, Oroslavje, Croatia
  HON Honeywell Technologies Sarl, Ecublens, Switzerland
  HPL HPL-Socomec Pvt. Ltd., 133 Pace City 1, Sector 37, Gurgaon, India
  HRM Hefei Runa Metering Co., Ltd, 1102# jinchi Rd. Luyang industrial park, Hefei, Anhui Province, Hefei, CHINA
  HRS HomeRider SA, France
  HSD Ningbo Histar Meter Technology Co.,Ltd., No.181 Haichuan Road Jiangbei District, Ningbo City, Zhejiang Province, CHINA
  HST HST Equipamentos Electronicos Ltda
  HTC Horstmann Timers and Controls Ltd.
  HTL Ernst Heitland GmbH & Co. KG, Erlenstr. 8-10, 42697 Solingen, Deutschland
  HTS HTS-Elektronik GmbH
  HWT Huawei Technologies Co. Ltd., Department of Industry Standards, Huawei Industrial Base, Shenzhen, China
  HXD Beijing HongHaoXingDa Meters CO.,LTD, HouXing,the third street,18,HuoXian, TongZhou., Beijing, China(P.R.C)
  HXE Hexing Electrical Co., Ltd, Hangzhou, China
  HXW Hangzhou Xili Watthour Meter Manufacture Co. Ltd., No. 14, JiaQi Road, XianLin Industrial Park, Yuhang District, Hangzhou, China
  HYD Hydrometer GmbH
  HYE Zhejiang Hyayi Electronic Industry Co Ltd, Zhejiang, China
  HYG Hydrometer Group, 91522 Ansbach, Germany
  HZC TANGSHAN HUIZHONG INSTRUMENTATION CO., LTD., Qinghua Road,New and Hi-Tech Development, Zone,Tangshan,Hebei Province,China, Tangshan, China
  HZI TANGSHAN HUIZHONG INSTRUMENTATION CO., LTD., Qinghua Road,New and Hi-Tech Development, Zone,Tangshan,Hebei Province,China, Tangshan, China
  ICM Intracom, Greece
  ICP PT Indonesia Comnets Plus, PLN building 9th Floor Jl.Jendral Gatot Subroto kav.18, Jakarta Selatan, Indonesia
  ICS ICSA (India) Limited, Plot No. 12, 1st Floor, Software units Layout, Cyberabad, Hyderabad, India
  ICT International Control Metering-Technologies GmbHWillhoop 7, D-22453 Hamburg, Germany
  IDE IMIT S.p.A
  IEC leonnardo Corporation, Peremogy, 31, Sutysky, Ukraine
  IEE I.E. Electromatic, S.L, Quart de Poblet (Valencia), Spain
  IFX Infineon Technologies, AM Campeon 1-12, Nuebiberg, Germany
  IHM Shenzhen Inhemeter Co., Ltd., 7/F, Science & Industry Park Building, Science & Industry Park, Nanshan District, Shenzhen, China
  IJE ILJIN Electric, Kyunggi-Do, Korea
  IMS IMST GmbH, Carl-Friedrich-Gauss-Straße 2-4, 47475 Kamp-Lintfort, Germany
  INC Incotex, 16th Parkovaya st, 26, Moscow, Russia
  IND INDRA SISTEMAS, Avda. Bruselas, 35, Alcobendas (Madrid)
  INE INNOTAS Elektronik GmbH, Rathenaustr. 18a, 02763 Zittau, Germany
  INF Infometric, Sollentunavägen 50, 19140 Sollentuna, Sweden
  INI Altero AB, 21211 Malmoe, Sweden
  INP INNOTAS Produktions GmbH, Rathenaustr. 18a, 02763 Zittau, Germany
  INS INSYS MICROELECTRONICS GmbH, Hermann-Köhl-Str. 22, 93049, Regensburg, GERMANY
  INT Infranet Technologies GmbH, 21079 Hamburg, Germany
  INV Sensus Metering Systems, Ludwigshafen/Rh, Germany
  INX Innolex Engineering BV, Molenlei 2A, Akersloot, The Netherlands
  IPD IPD Industrial Products Australia, Sydney, Australia
  ISI Akcionarsko Drustvo "Insa Industrija Satova, Trscanska 21, Belgrade-Zemun, Serbia
  ISK Iskraemeco, Slovenia
  ISO Isoil Industria spa, via F.lli Gracchi n.27, Cinisello Balsamo (Milan), Italy
  IST Ista
  ITA iTrona GmbH, CH-6432 Rickenbach SZ, Switzerland
  ITB ITRON Brazil, rua Fioravante Mancino, 1560, CEP: 13175-575 Sumaré, Brazil
  ITC INTECH TUNISIE, Rue de Tozeur ZI Hammam Zriba, Zaghouan, Tunisia
  ITE ITRON (Electricity), 52, Avenu Camille Desmoulin, 92130 Issy les Moulineaux, FRANCE
  ITG ITRON (Gas), 52, Avenue Camille Desmoulin, 92130 Issy les Moulineaux, FRANCE
  ITH INTELTEH d.o.o., Bozidara Magovca 87, 10000, Zagreb, Croatia
  ITF ITF Fröschl GmbH, Hauserbachstrasse 9, 93194 Walderbach, Germany
  ITI ITRON Asia, EJIP Plot 6B-2, Lemah Abang, Bekasi 17550, Jawa Barat, Indonesia
  ITR Itron
  ITS ITRON Australia, Rosberg Road, Wingfield, SA, 5013, Adelaide, Australia
  ITU ITRON United States, 2111 N Molter Road, Liberty Lake, WA 99019, United States
  ITW ITRON (Water), 52, Avenue Camille Desmoulin, 92130 Issy les Moulineaux, FRANCE
  ITZ ITRON South Africa, Tygerberg Office Park, Hendrik, Verwoerd Drive, 7500 Plattekloof, Cape Town, South Africa
  IUS IUSA SA DE CV, Km 109 Carr Panamericana, Pasteje Jocotitlan Edo. de Mex., Mexico
  IWK IWK Regler und Kompensatoren GmbH
  IZE iZenze AB, Slottagårdsgatan 9, 235 35 Vellinge, Sweden
  JAN Janitza electronics GmbH, Lahnau-Waldgirmes, Germany
  JCE Janz Contadores de Energia SA, Lisbon, Portugal
  JED JED Co Ltd, Dongan-Gu, Anyang, Kyunggi-Do, South Korea
  JGF Janz Contagem e Gestao de Fluidos SA, Lisbon, Portugal
  JHM Changzhou Jianhu Intelligentize Meter Co.,Ltd., No.11 Lijia Industrial District,Wujin, Changzhou, China
  JMT JM-TRONIC Sp. z o.o., ul. Wybrze e Ko ciuszkowskie 31/33, 00-379 Warszawa, Poland
  JOY Zhejiang Joy Electronic Technology Co., Ltd., No. 333 North Chayuan Road, Youchegang Town, Xiuzhou District, Jiaxing, China
  JUM JUMO GmbH & Co. KG, Herrmann - Muth - Strasse 1, 36039 Fulda, Germany
  KAM Kamstrup Energi A/S
  KAS Kamstrup A/S, Industrivej 28, 8660 Skanderborg, Denmark
  KAT KATHREIN-Werke KG, Anton-Kathrein-Straße 1-3, D-83022 Rosenheim, Germany
  KBN Alpamis IT Ltd, ANKARA, Turkey
  KEL KELEMINIC d.o.o., Zagreb, Croatia
  KER KERMS UG (haftungsbeschränkt), Fontanestraße 39, 15569 Woltersdorf, Germany
  KFM Shenzhen KAIFA Technology Co, Ltd, Shenzhen, China
  KGE Guangzhou Keli General Electric Co., Ltd, No1.Sui Hua Nan street, Jiang Nan Da Dao Zhong Road, Guangzhou P.R.China
  KHL Kohler, Turkey
  KKE KK-Electronic A/S
  KMB Kamstrup A/S, Industrivej 28, Stilling, DK 8660 Skanderborg, Denmark
  KMT Krohne Messtechnik GmbH, Ludwig-Krohne-Straße, Duisburg,Germany
  KNX KONNEX-based users (Siemens Regensburg)
  KRO Kromschroder
  KST Kundo SystemTechnik GmbH
  KSY KUNDO SystemTechnik GmbH, St Georgen, Germany
  KTC Kerman Tablo Co, Tehran, Iran
  LAC Heinz Lackmann GmbH & Co KG, Harkortstrasse 15, 48163 Münster, Germany
  LAN Langmatz GmbH, Am Gschwend 10, Garmisch-Partenkirchen, Germany
  LCG Landis+Gyr Meter & System (Zhuhai) Co., Ltd, No.12 Pingdong 3RD,Nanping Industry Community,Zhuhai 519060, P.R.China
  LCR ShanDong LiChuang Science and Technology Co.,Ltd, No. 9 Fenghuang Road High-tech District, Laiwu ShandongChina
  LDE Shenzhen Londian Electrics Co., Ltd, 3/F, Build 107#, 1st Nanyou Industrial Zone, Nanshan District, Shenzhen, China
  LEC Lectrotek Systems Pvt Ltd, 33 Parvati Industrial Estate, 411009, Pune, India
  LEM LEM HEME Ltd., UK
  LGB Landis+Gyr Ltd., UK
  LGD Landis+Gyr GmbH, Germany
  LGS Landis+Gyr (Pty) Ltd. South Africa
  LGU LG Uplus Corp, Namdaemunno 5-ga, Jung-gu, Seoul, Korea
  LGZ Landis+Gyr AG Zug
  LHA Atlantic Meters, South Africa
  LML LUMEL, Poland
  LNT Larsen & Toubro Ltd, MPS, KHebbal-Hootagalli, Mysore, India
  LSE Landis & Staefa electronic
  LSK LS Industrial Systems Co Ltd, Cheongju, South Korea
  LSP Landis+Gyr GmbH, Germany
  LSZ Siemens Building Technologies
  LUG Landis+Gyr GmbH, Germany
  LUN Protokol Sanayi ve Ticaret, Karacaoglan Mah., 167 Sok., No 42 Isikkent, Izmir, Turkey
  LYE Jiangsu Linyang Electronics Co.,Ltd., No.666,Linyang Road, Qidong, China
  MAC RUDNAP Group Meter & Control, Omladinskih brigada 182, Belgrade, Serbia
  MAD Maddalena S.p.A.. Italy
  MAE Mates Elektronik Metin Ates, Ankara, Turkey
  MAN Manthey GmbH, Walter-Freitag-Str. 30, 42897 Remscheid, Deutschland
  MAT Mitsubishi Electric Automation, Bangkok, Thailand
  MAX MAXMET Inc, Seogu, Daejeon 302-834, Korea
  MBS MBS AG, Eisnachstraße 51, 74429 Sulzbach-Laufen, Germany
  MCR MICRORISC, D lnická 222, Ji ín, Czech Republic
  MDE Diehl Metering Deutschland, Industriestraße 13, Ansbach, Germany
  MEC Mitsubishi Electric Corporation, 1-8, Midorimachi Fukuyama-city Hiroshima, 720-8647, Japan
  MED MAHARASHTRA STATE ELECTRICITY DISTRIBUTION COMPANY LIMITED, PLOT NO. G-9, PRAKASHGAD, 5TH FLOOR, PROF. ANANT KANEKAR MARG, BANDRA (EAST)MUMBAI 400051, INDIA
  MEH Mueller-electronic GmbH, Fritz-Garbe-Str. 2, 30974, Wennigsen, Germany
  MEI Sensus Metering Systems, Ludwigshafen/Rh, Germany
  MEL Mikroelektronika a.d, Banja Luka, Bosnia and Herzegovina
  MEM MEMS AG, Segelhofstrasse, CH-5405 Baden-Dättwil, Switzerland
  MET METRA Energie-Messtechnik GmbH, Speyer, Germany
  MIC Microtronics Engineering GmbH, Hauptstrasse 7, A-3244 Ruprechtshofen, Austria
  MII Miitors ApS, Miitors ApS, VBI Park, Chr. M Østergaardsvej 4aDK-8700 Horsens, Denmark
  MIM Malaysian Intelligence Meters Sdn. Bhd., No. 3, Jalan Pemberita U1/49, Temasya Industrial Park, Seksyen U1, Glenmarie Shah Alam, Selangor Darul Ehsan, Malaysia
  MIR MIR Research and Production Association, 51 Uspeshnaya644105 Omsk, Russia
  MIS Iskra MIS d.d, 4000 Kranj, Slovenia
  MKE MKEK Genel Mudurlugu Gazi Fisek (ELSA), Ankara, Turkey
  MKL MAKEL Elektrik Malzemeleri, Turkey
  MKS MAK-SAY Elektrik Elektronik, Turkey
  MMC Modern Meters Co., Damascus Sahnaya, Syria,
  MMI MyMeterInfo, 95 rue du Morellon, 38070 Saint Quentin, Fallavier, France
  MMS Brunswick Bowling and Billiards UK Ltd, Unit L1, Temple Court, Knights Park, Knight Road, Strood, Kent, UK
  MNS MANAS Elektronik, Turkey
  MOS MOMAS SYSTEMS NIGERIA LIMITED, #4, Bode Thomas Street, Surulere, Lagos, NIGERIA
  MPA Mega Power Automation International Limited, 16/F., Block A-1, Fortune Factory Building, 40 Lee Chung Street, Chai Wan, Hong Kong.
  MPR Michael Rac GmbH, Sonnenfeld 29, 91522 Ansbach, Germany
  MPS Multiprocessor Systems Ltd, Bulgaria
  MRT MIRTEK LTD., Gagarin street, Building 4, Stavropol, Russia
  MSB MISA SDN BHD, LOT 30, JALAN MODAL 23/2, 40300, SHAH ALAM, SELANGOR, MALAYSIA
  MSE Mahashakti Energy Limited, A-8, New Focal Point, Dabwali Road, Bathinda (Punjab), India
  MSM MS-M Co., Ltd., 237 Bukjung-Dong Yangsan-City, Korea
  MSO Metiso, Tr anska 21, Zemun, Belgrade, Serbia
  MSY MAK-SAY Elektrik Elektronik Malzemeleri, Turkey
  MTD Removed - November 2006
  MTC Metering Technology Corporation, USA
  MTH njmeter, Binjiang Development Zone of Jiangning Road No. 6Nanjing, China
  MTI Micrtotech Industries Pakistan, Plot#2,Street#2,Attari industrial estate 18-Km. Ferozepure Raod, Lahore, Pakistan
  MTM Metrum Sweden AB, Vestagatan 2A, Gothenburg, Sweden
  MTS MeteRSit S.r.L., Viale dell"Industria, 31, Padova, Italy
  MTX Matrix Energy Pvt. Ltd., Soni Arcade, No. 242, 2nd Floor, 7th Cross, 6th Block, Banashankari 3rd Stage, Bangalore, INDIA
  MUK Meters UK Ltd, Whitegate, White Lund Trading Estate, Lancaster, UK
  MWU METRONA Wärmemesser Union GmbH, Aidenbachstraße 40, 81379 München, Germany
  MXM Maxim India Integrated Circuit Design Pvt Ltd., 132/133, Divyasree Technolopolis, Off Airport Road, Bangalore, India
  NAR NARI Group Corporation-NARI Technology Development Co., Ltd, No.8 NARI Rd. Gulou District, Nanjing, China
  NDF NÚCLEO DURO FELGUERA, Avda. de la Industria, 24, 28760, Tres Cantos, Madrid
  NDM Northern Design, 228 Bolton Road, Bradford, United Kingdom
  NES NORA ELK. MALZ. SAN. ve T C. A. ., nönü Cad. Sümer Sok. Zita Merkezi C1 Blok No:9 Kozyata -Kad köy- STANBUL, TURKEY
  NIS Nisko Industries Israel
  NJC NAMJUN Co Ltd, Gimhae Gyoungnam, South Korea
  NMS Nisko Advanced Metering Solutions Israel
  NNT 2N Telekomunikace a.s., Modranska 621, 14301 Praha 4, Czech Republic
  NRM Norm Elektronik, Turkey
  NTC Nuri Telecom Co Ltd, Geumcheon-gu, Seoul, Korea
  NTM Netinium, Postbus 86, Wormerveer, The Netherlands
  NVN NOVEN ENERGY AND ICT LTD., Hacettepe University KOSGEB Technology Center T1-Blok B:14 Beytepe, Ankara,Turkiye
  NWM Ningbo Water Meter Co.Ltd., No.99 Lane 268 Beihai Road Jiangbei District, Ningbo City Zhejiang ProvinceCHINA
  NXP NXP Semiconductors, High Tech Campus 32, 5656AE Eindhoven, The Netherlands
  NYG Ningbo Yonggang Instrument Co.,Ltd, Weisan Road, West Industrial Zone, Xinpu Town, Cixi City, China
  NYN Nanjing Yuneng Instrument Co Ltd, Nanjing, China
  NZR Nordwestdeutsche Zählerrevision Ing. Aug. Knemeyer GmbH & Co. KG, Heideweg 33, 49196, Bad Laer, Germany
  OAS Omni Agate Systems, Chennai, India
  ODI OAS Digital Infrastructures Pvt. Ltd., No:4/3, Stringer Road, Periamet, Vepery, Chennai, INDIA
  OEE ONUR Elektrik ve Elektronik, Turkey
  OMS OMNISYSTEM Co., Ltd., Goyang-shi, Gyeonggi-do, Korea
  ONR ONUR Elektroteknik, Turkey
  ONS ONUR Elektroteknik, Turkey
  OPT Optec GmbH, Grundstrasse 22, 8344 Bäretswil, Switzerland
  ORB ORBIS Tecnologia Electrica, SA, Madrid, Spain
  ORM Ormazabal, B Basauntz, 2, Igorre, Spain
  OSA Osaki Electric Co., Ltd.(Europe), Gotanda-Square, Tokyo, Japan
  OSK Osaki Electric Co Ltd. (Japan), Shinagawa-ku, Tokyo, Japan
  OZK Oz-kar Enerji, Kayseri, Turkey
  PAD PadMess GmbH, Germany
  PAF FAP PAFAL S.A., 26 Lukasinskiego street, widnica, Poland
  PAK Paktim Energy Sp.zo.o., ul. Swiety Marcin 29/8, 61-806 Poznan, Poland
  PAN Panasonic Corporation, 800 Tsutsui-cho, Yamatokoriyama-shiNara Pref., Japan
  PDX Paradox Engineering SA, Via Ronchetto, 9, Cadempino, Switzerland
  PEL Pak Elektron Ltd. (PEL), 14-km Ferozpur Road, Lahore, Pakistan
  PGP P.G.P. - Smart Sensing s.a., Rue Fond Cattelain 2 / 1.15, Mont-St-Guibert, Belgium
  PHL HangZhou PAX Electronic Technology Co., Ltd., China
  PII PiiGAB Processinformation i Goteborg AB, Sweden
  PIK pikkerton GmbH, Kienhorststrasse 70, 13403 Berlin, Germany
  PIL Pilot Systems (London) Ltd, Chiswick, London
  PIM Power Innovation GmbH, Rehland 2, Achim,Germany
  PIP Hermann Pipersberg jr., Felder Hof 2, D-42899 Remscheid, Deutschland
  PLO Weihai Ploumeter Co. Ltd., : No. 28 Hengrui Street, Torch Hi-Tech Industries Development Zone, Weihai, Shandong, China
  PMG Sensus Metering Systems, Ludwigshafen/Rh, Germany
  POD PowerOneData, Bangalore 560082, India
  POW PowerApp, Esromgade 15 opg. 2 - 2 sal., Copenhagen, Denmark
  POZ ZEUP Pozyton sp. z o.o, ul. Czestochowa, Poland
  PPC Power Plus Communications AG, Am Exerzierplatz 2, 68167 Mannheim, Germany
  PRE Predicate Software, 7 Protea Ave, Dooringkloof, 0140, Centurion, Gauteng, South Africa
  PPS Palace Power Systems, 50 Oak Avenue, Pretoria, South Africa
  PRI Polymeters Response International Ltd.
  PRG Paud Raad Industrial Group, No. 18, 2nd St., Shah Nazari Ave.,Madar Sq., Mirdamad Blvd., Tehran, Iran
  PRO Proton - Elektromed Ltd, Ankara, Turkey
  PST PSTec Co.,Ltd, Seoul, Korea
  PWB Paul Wegener GmbH, Ballenstedt, Germany
  PWR Powrtec, Scotts Valley, CA 95066, USA
  PYU PYUNGIL Co. Ltd, Anyang-si, Gyeonggi-do, Korea
  QDS Qundis GmbH, Sondershaeuser Landstrasse 27, Mühlhausen, Germany
  QTS QT systems ab, Alfavägen 3, 92133 Lycksele, Sweden
  RAC Michael Rac GmbH, Sonnenfeld 29, Ansbach, Germany
  RAD Radiocrafts AS, Sandakerveien 64, 0484 OSLO, NORWAY
  RAM Rossweiner Armaturen und Messgeräte GmbH & Co. OHG, Wehrstraße 8, Roßwein, Germany
  RAS Rubikon Apskaitos Sistemos, Vilnius, Lithuania
  RCE RC ENERGY METERING PVT,LTD., B-65 GATE NO.1,N.I.A.PHASE-II NEW DELHI -110028, DELHI, INDIA
  REC Zhejiang Reallin Electron Co.,Ltd, 2F,Building3,No.202 Zhenzhong Rd,Sandun Technology Park,Xihu District, Hangzhou, China
  REL Relay GmbH, Germany
  REM Remote Energy Monitoring, Tring, UK
  RIC Richa Equipments Pvt. Ltd., Z B 5-6/487, Zulfe Bengal, Dilshad Garden Shahdara, Delhi, India
  RIL Rikken Instrumentation Limited, Plot No. 369, Phase 2, Industrial Area, Panchkula, India
  RIM CJSC \"Radio and Microelectronics\", 630082, Novosibirsk, the Red Prospectus, 220, the case 17, Novosibirsk, Russia
  RIT Ritz Instrument Transformers GmbH, Wandsbeker Zollstr. 92 98, 22041 Hamburg, Germany
  RIZ RIZ Transmitters, Bozidareviceva 13, Zagreb, Croatia
  RKE Viterra Energy Services (formerly Raab Karcher ES)
  RML ROLEX METERS RPIVATE LIMITED, Plot No 20&21, Prashanthi Nagar, Kukatpally Industrial Estate, Hyderabad, India
  RMR Advanced Technology RAMAR, Christchurch, UK
  RSA Rahrovan Sepehr Andisheh Pte. Co.
  RSW RSW Technik GmbH, Giessen, Germany
  SAA Sanjesh Afzar Asia Ltd. Co., 3 rd Flr/No. 8/16 St./Gandi Ave./Tehran, Iran
  SAC Sacofgas 1927 SpA, Via Ascanio Sforza 85, Milano, Italy
  SAG SAGEM, Cergy Saint-Christophe, France
  SAM Siemens AG Österreich, AMIS (Automated Metering and Information System), Ruthnergasse 3, Vienna, Austria
  SAP Sappel
  SAT SATEC Ltd, 7 Hamarpe Street, Jerusalem, Israel
  SBC Saia-Burgess Controls, Bahnhofstrasse 18, 3280 Murten, Switzerland
  SCE Seo Chang Electric Communication Co Ltd, Daegu, Korea
  SCH Schinzel GmbH
  SCW ScatterWeb GmbH, Charlottenstr. 16, Berlin, Germany
  SDC SdC Sistemas de Contagem, Vila Nova de Famalicao, PT
  SDM Shandong Delu Measurement Co., Ltd., Tower C, Qiln Software Park, High-Tech Industrial Development, JiNan, China
  SEC Schneider Electric Canada, Saanichton, Canada
  SEE El Sewedy Electrometer Egypt, 6th of October, Egypt
  SEN Sensus Metering Systems, Ludwigshafen/Rh, Germany
  SGA smartGAS Mikrosensorik GmbH, Kreuzenstraße 98, 74076 Heilbronn, Germany
  SGM Swiss Gas Metering AG, Reichenauerstrasse, Domat/Ems, Switzerland
  SHD Beijing SanHuaDeBao Energy Technoligy Co.,Ltd., Floor4 Jinyanlong R&D Building Jiancaicheng West Road Changping District Beijing City China
  SHE Shenzhen SingHang Elec-tech Co., Ltd., Rm203-206, Terra Science & Technology Park, Futian District, Shenzhen, China
  SHM Shanghai Metering, China, No.2065 Kongjiang Road, Shanghai, China
  SIE Siemens AG
  SIT SITEL doo, Belgrade, Serbia and Montenegro
  SLB Schlumberger Industries Ltd.
  SLP Sylop, ul. Jagiellonska 4, PL-32830 Wojnicz, Poland
  SLX Ymatron AG, Bruelstrasse 7, Dielsdorf, Switzerland
  SMC Pending
  SME Siame, Tunisia
  SMG Samgas s.r.l., SP 33 km 0,600 20080, Vernate (MI), Italy
  SML Siemens Measurements Ltd.
  SMT Smarteh d.o.o., Trg tigrovcev 1, Tolmin, Slovenia
  SNM ShenZhen Northmeter Co.Ltd , floor 5, Dongshan Building, Huafeng first science park,Baoan, Shenzhen, China
  SNR NTN-SNR, 1 Rue des Usines, 74010 Annecy, France
  SNS Signals and Systems India Private Limited, MF-7, Cipet Hostel Road, Thiru-Vi-Ka Industrial Estate, Chennai, India
  SOC SOCOMEC, 1, rue de Westhouse, 67230 Benfeld, France
  SOF Softflow.de GmbH, Dorfstasse, 15834 Gross Machnow, Germany
  SOG Sogecam Industrial, S.A., C/ Rosalind Franklin, 22-24, Campanillas (Málaga), Spain
  SOL Soledia Srl, Via di Selva Candida 85, Rome, Italy
  SOM Somesca, 80 rue Jean Jaures, 92270 Boois colombes, France
  SON Sontex SA
  SPL Sappel
  SPX Sensus Metering Systems, Ludwigshafen/Rh, Germany
  SRE Guangzhou Sunrise Electronics Development Co., Ltd., Guangzhou Avenue South, Guangzhou, Guangdong, China
  SRF Saraf Industries, Saraf Industries, Bathinda Road, Rampura Phul - 151103, Punjab, India
  SRN Shandong SARON Intelligent Technology Co., Ltd., 3F,South E,International Business Center, Environmental Technology Area, Middle of Zhengfeng Road, Jinan City Shandong Province,China
  SRV Servic LLC, Kirova 16-9, Dniprodzerzhinsk, Ukraine
  SSN Silver Spring Networks, 555 Broadway Street, Redwood City, United States
  SST Qingdao Haina Electric Automation Systems Co., Ltd., No.151,Zhuzhou Road, Laoshan, Qingdao, China
  STA Shenzhen Star Instrument Co Ltd, Shenzhen, China
  STC Sunrise Technology Co., Ltd, Building C, Xiyuan 8th Road 2#, West-Lake Technological & Economic Zone, Hangzhou, China
  STD Stedin, Essebaan 71, Capelle a/d Ijssel, Netherlands
  STR Strike Technologies, South Africa
  STV STV Automation, Branch of STV Electronic, Detmold, Germ.
  STZ Steinbeis Innovation Center Embedded Design and Networking c/oUniversity of Cooperative Education Loerrach Hangstrasse 46-50, D79539 Loerrach
  SVM AB Svensk Värmemätning SVM
  SWI Swistec GmbH, Graue-Burg-Strasse 24-26, Bornheim, 53332 Germany
  SWT Beijing Swirling Technology Co. Ltd, Beijing, China
  SYN SMSISTEM Ltd., ANKARA, Turkey
  TAG Telma AG, Gewerbeweg 10, 3662 Seftigen, Switzerland
  TAT Tatung Co., 22, Chungshan N. Rd., 3rd Sec., Taipei, Taiwan
  TAY Taytech Otomasyon ve Bilisim Teknolojileri LTD. Sti, Tasdelen Gungoren Mah. Izan Sok. No:15 Cekmekoy., Istanbul, Turkey
  TCE Qindao Techen Electronic Technology Co.,LTD, No. 169 Songling Road. Laoshan District, Qingdao, China
  TCH Techem Service AG & Co. KG
  TDC Telecom Design, Rue Romaine Voie de Remora, GradignanFRANCE
  TEC TECSON Digital, Felde, Germany
  TEP TEPEECAL, 69730 Genay, France
  TFC Toos Fuse Co. 375 Sanat Blvd. Toos Industrial Estate, Mashad, Iran
  THE Theben AG, Hohenbergstrasse 32, 72401 Haigerloch, Germany
  TIP TIP GmbH, Bahnhofstr. 26, 99842 Ruhla, Germany
  TIX Tixi.Com GmbH, D-13465 Berlin
  TLM Theodor Lange Messgeräte GmbH, Rodeberg 7, 31226 Peine,Germany
  TMK Timi Kosova Sh.p.k.
  TMS TEMASS IMALAT A.S, Macunkoy, Ankara, Turkey
  TPL Teplocom Holding, 45, Vyborgskaya Naberezhnaya, ST Petersburg, Russian Federation
  TRI Tritech Technology AB, Sturegatan, 10-12 PO Box 1094, SE-172 22 Sundbyberg, Stockholm, Sweden
  TRJ SHENZHEN TECHRISE ELECTRONICS CO.,LTD, Building 112,1st Industrial park, Liantang, Luohu District, Shenzhen City, China.
  TRL Trilliant Inc., 610 du Luxembourg, Granby, (Quebec), Canada, J2J 2V2
  TRV Transvaro Elektron Aletleri A.S., Turkey
  TSD Theobroma Systems Design und Consulting GmbH, Gutheil-Schoder Gasse 17, Wien, Austria
  TTM Toshiba Toko Meter Systems Co., Ltd., 12-7, Shiba 1-chome, Minato-ku, TOKYO, JAPAN
  TTR Tetraedre Sarl, Epancheurs 34b, 2012 Auvernier, Switzerland
  TTT Telephone and Telegraph Technique Plc, Sofia, Bulgaria
  TUR TURKSAY ELEKTRONIK ELEKTRIK ENDUSTRISI
  TXL CETC46 TianJin New Top Electronics Technology Co.,Ltd., KEYAN East Road 15,Nankai District, Tianjin, China
  UAG Uher
  UBI Ubitronix system solutions gmbh, 4232 Hagenberg, Austria
  UEI United Electrical Industries Limited, Pallimukku, Kollam, India
  UGI United Gas Industries
  UTI Utilia Spa, Via Chiabrera, 34/D, Rimini (RN 47924), Italia
  UTL United Telecoms Limited, A-1/A, 2nd Floor, Revati Building, ECIL XRoads. Hyderabad, INDIA
  VER VERAUT GmbH, Siemensstr.52, Linz, Austria
  VES Viterra Energy Services
  VIK VI-KO ELEKTRIK, Istanbul, Turkey
  VMP VAMP Oy., Yrittäjänkatu 15 P.O. Box 810 Fl-65101, Vaasa, Finland
  VPI Van Putten Instruments B.V.
  VSE Valenciana Smart Energy of Mediterranean Sea S.A, Sir Alexander Fleming, 12 . Warehouse 11, Parque tecnologico de Valencia, 46980, Valencia, Spain
  VTC Vitelec, Kapittelweg 18, NL 4827 HG Breda, Postbus 6543, NL 4802 HM Breda, Netherlands
  VTK Linkwell Telesystems Pvt. Ltd., Gowra Kalssic, 1-11-252/1/A, Begumpet, Hyderabad 500016, India
  YTL ZheJiang yongtailong electronic co.,ltd, No.8 KangDing Road, Tongxiang, China
  WAH WAHESOFT UG, Moeoerte 16, 26316 Varel, Germany
  WAI Chongqing WECAN Precision Instruments Co.,Ltd, #66 HuangShan Rd,HI-Tech Park , New North Zone, ChongqingP.R.China
  WAL Wallaby Metering Systems Pvt. Ltd., M-3, 9th Street, Dr.VSI Estate, Thiruvanmiyur, Chennai - 600 041, INDIA
  WEB Webolution GmbH & Co. KG, Sendenhorsterstrasse 32, 48317 Drensteinfurt, Germany
  WEG WEG Equipamentos Elétricos S.A. Automação, Av. Pref. Waldemar Grubba, 3000, Jaraguá do Sul, Brazil
  WEH E. Wehrle GmbH, Obertalstraße 8, 78120 Furtwangen, Germany
  WEL WELLTECH automation, 263# HongZhong Road, Shanghai,P.R.China
  WFT Waft Embedded Circuit Solutions, A-109, Sahni Tower, Sector-5, Rajendra Nagar, Sahibabad, Ghaziabad (U.P.), India
  WIN Windmill Innovations BV, Paasbosweg 14-16, 3862 ZS, Nijkerk (GLD), The Netherlands
  WMO Westermo Teleindustri AB, Sweden
  WSD Yantai Wisdom Electric Co., Ltd., JiChang road 2#, Yantai, Shangdong Province, China
  WSE Changsha Weisheng Electronics Ltd, Changsha, P.R. China
  WTI Weihai Sunts Electric Meter Co., Ltd, 39-7#, Shenyang Middle Rd. Gaoji Weihai CHINA
  WTL Wipro Technologies, Doddakannelli, Sarjapur Road. Bangalore, India
  WTM Watertech S.r.l., Strada dell"Antica Fornace, 2/4, 14053 Canelli (At) Italy
  WZG Neumann & Co. Wasserzähler Glaubitz GmbH, Industriestraße A7, 01612 Glaubitz, Germany / Deutschland
  WZT Wizit Co Ltd, Ansin-City Gyeonggi-Do, S Korea
  XAO Info Solution SpA, Via della Burrona, 51, Vimodrone (MI), Italy
  XEM XEMTEC AG, Sarnen, Switzerland
  XJM XJ Metering Co., Ltd, No 416, Ruixiang Road, Xuchang, Henan, China
  XMA XMASTER s.c. ul. Gersona 41, Wroclaw, Poland
  XMX Xemex NV, B-2900 Schoten, Belgium
  XTR HENAN SUNTRONT TECH CO., LTD, No.19 Guohuai Street, High and New Tech Industrial Development Zone, Zhengzhou City, Henan Province, China
  XTY LianYuanGang Tengyue Electronics & Technology Co.LianYunGang.Jiangsu.china Haizhou, LianYunGang, China
  YDD Jilin Yongda Group Co., Ltd
  YHE Youho Electric Co Ltd, Yangjoo, South Korea
  YSS Yellowstone Soft, Brunnenstr. 32, 89584 Ehingen, Germany
  YTE Yuksek Teknoloji, Turkey
  ZAG Zellweger Uster AG
  ZAP Zaptronix
  ZEL Dr. techn. Josef Zelisko GmbH, Beethovengasse 43 45A-2340 Mödling, Austria
  ZIV ZIV Aplicaciones y Tecnologia, S.A.
  ZJY Zhejiang Jiayou Thermal Technology Equipment Co., LTD, Houwan Industrial Point,Yucheng Street,Yuhuan County, Taizhou, China
  ZPA ZPA Smart Energy a.s., Komenského 821, CZ-541 01 TrutnovCzech Republic
  ZRI ZENNER International GmbH & Co. KG, Postfach 10 33 39D-66033 Saarbrücken, Germany
  ZRM ZENNER International GmbH & Co. KG, Postfach 10 33 39D-66033 Saarbrücken, Germany
  ZTY Hangzhou Chint Meter Technology Co., Ltd, 7th Floor, New Building, No 313, Tianmushan Road, Hangzhou 310013, China
 *
 */
const char * GetManufacturerFullString(char * szMCode)
{
    if(!szMCode) return "";

    if(strlen(szMCode) < 3) return "UNKNOWN";

    if(!strncmp(szMCode, "ABB", 3)) return "ABB AB, P.O. Box 1005, SE-61129 Nyköping, Nyköping,Sweden";
    if(!strncmp(szMCode, "ABN", 3)) return "ABN Braun AG, Platenstraße 59, 90441 Nürnberg, Germany";
    if(!strncmp(szMCode, "ACA", 3)) return "Acean, Zi de la Liane, BP 439, 62206 Boulogne Sur Mer Cedex, FRANCE";
    if(!strncmp(szMCode, "ACE", 3)) return "Actaris, France. (Electricity)";
    if(!strncmp(szMCode, "ACG", 3)) return "Actaris, France. (Gas)";
    if(!strncmp(szMCode, "ACW", 3)) return "Actaris, France. (Water and Heat)";
    if(!strncmp(szMCode, "ADD", 3)) return "ADD-Production S.R.L., 36, Dragomirna str., MD-2008, Chisinau, Republic of Moldova";
    if(!strncmp(szMCode, "ADN", 3)) return "Aidon Oy, 40101 Jyvaskyla, Finland";
    if(!strncmp(szMCode, "ADU", 3)) return "Adunos GmbH, Am Schlangengraben 16, D-13597 Berlin, Germany";
    if(!strncmp(szMCode, "ADX", 3)) return "ADD-Production S.R.L., 36, Dragomirna str., MD-2008, Chisinau, Republic of Moldova";
    if(!strncmp(szMCode, "AEC", 3)) return "Advance Electronics Company, Riyadh, Saudi Arabia ";
    if(!strncmp(szMCode, "AEE", 3)) return "Atlas Electronics, 17530 Surdulica, Serbia and Montenegro";
    if(!strncmp(szMCode, "AEG", 3)) return "AEG";
    if(!strncmp(szMCode, "AEL", 3)) return "Kohler, Turkey";
    if(!strncmp(szMCode, "AEM", 3)) return "AEM S.C. AEM S.A. Romania";
    if(!strncmp(szMCode, "AER", 3)) return "Aerzener Maschinenfabrik GmbH, Reherweg 28, 31855 Aerzen, Germany";
    if(!strncmp(szMCode, "AFX", 3)) return "Alflex Products, Zoetermeer, Holland";
    if(!strncmp(szMCode, "AGE", 3)) return "AccessGate AB, Rissneleden 144, 174 57 Sundbyberg, Sweden";
    if(!strncmp(szMCode, "ALF", 3)) return "Alfatech - Elektromed Elektronik, Ankara, Turkey";
    if(!strncmp(szMCode, "ALG", 3)) return "Algodue Elettronica srl, Via Passerina, 3/A, Fontaneto D'Agogna, Italy";
    if(!strncmp(szMCode, "ALT", 3)) return "Amplitec GmbH, Gootkoppel 28, Reinfeld, Germany";
    if(!strncmp(szMCode, "AMB", 3)) return "Amber wireless GmbH, Hawstra e 2a, 54290 Trier, Germany";
    if(!strncmp(szMCode, "AME", 3)) return "AVON METERS PRIVATE LIMITED, D-15/16/17, INDUSTRIAL FOCAL POINT, DERABASSI,PUNJAB-140507INDIA";
    if(!strncmp(szMCode, "AMH", 3)) return "AMiHo Ltd, 1010 Cambourne Business Park, Cambourne, Cambridge, CB1 9AY, UK";
    if(!strncmp(szMCode, "AMI", 3)) return "AMI Tech(I) Pvt. Ltd, #205&206,NSIC-EMDBP, Kamalanagar, ECIL PO, Hyderabad- 500062, India";
    if(!strncmp(szMCode, "AML", 3)) return "Eon Electric Ltd., C-124, Hosierry Complex, Noida Phase II, NOIDA, INDIA";
    if(!strncmp(szMCode, "AMP", 3)) return "Ampy Automation Digilog Ltd";
    if(!strncmp(szMCode, "AMS", 3)) return "Zhejiang Chaoyi Electronic Technology Co. Ltd., Zhejiang, China.";
    if(!strncmp(szMCode, "AMT", 3)) return "Aquametro";
    if(!strncmp(szMCode, "AMX", 3)) return "APATOR METRIX SA, Piaskowa 3, 83-110 Tczew, Poland";
    if(!strncmp(szMCode, "ANA", 3)) return "Anacle Systems PTE LTD, 1A International Business Park #05-02, Singapore";
    if(!strncmp(szMCode, "AND", 3)) return "ANDIS sro, Bratislava, Slovakia";
    if(!strncmp(szMCode, "APA", 3)) return "APATOR SA (Electricity), Zolkiewskiego 21/29, 87-100, Torun,Poland";
    if(!strncmp(szMCode, "APR", 3)) return "Apronecs Ltd, Gabrovo, Bulgaria";
    if(!strncmp(szMCode, "APS", 3)) return "Apsis Kontrol Sistemleri, Turkey";
    if(!strncmp(szMCode, "APT", 3)) return "Apator SA (Gas, water and heat), ó kiewskiego 21/29, Toru , Poland";
    if(!strncmp(szMCode, "APX", 3)) return "Amplex A/S, Aarhus C, Denmark";
    if(!strncmp(szMCode, "AQM", 3)) return "Aquametro AG, Ringstrasse 75, Therwil, Switzerland";
    if(!strncmp(szMCode, "AQT", 3)) return "AQUATHERM P.P.H, Kujawinski, Lomianki, Poland";
    if(!strncmp(szMCode, "ARC", 3)) return "Arcelik AS., Istanbul, Turkey";
    if(!strncmp(szMCode, "ARF", 3)) return "ADEUNIS_RF, 283 rue Louis NEEL, CROLLES 38920, France";
    if(!strncmp(szMCode, "ARM", 3)) return "arivus metering GmbH, Mielestr. 2, 14542 Werder (Havel), Germany";
    if(!strncmp(szMCode, "ARS", 3)) return "ADD - Bulgaria Ltd, Bul. '6 septemvri' 252 Et.7, Plovdiv, Bulgaria";
    if(!strncmp(szMCode, "ART", 3)) return "Electrotécnica Arteche Smart Grid, Derio Bidea 28, Zabalondo Industrialdea, 48100 Mungia, Bizkaia. Spain";
    if(!strncmp(szMCode, "ASR", 3)) return "Erelsan Elektrik ve Elektronik, Malzeme, Istanbul, Turkey";
    if(!strncmp(szMCode, "AST", 3)) return "ASTINCO Inc., 114 Anderson Ave. Suite 7A, ON, L6E1A5, Markham, Canada";
    if(!strncmp(szMCode, "ATF", 3)) return "AKTIF Otomasyon ve GS ve Tic, Turkey";
    if(!strncmp(szMCode, "ATI", 3)) return "ANALOGICS TECH INDIA LIMITED, Plot No.9/10, Road No.6, Nacharam Industrial Estate, HYDERABAD, INDIA";
    if(!strncmp(szMCode, "ATL", 3)) return "Atlas Elektronik, ANKARA, Turkey";
    if(!strncmp(szMCode, "ATM", 3)) return "Atmel, Torre C2, Polígono Puerta norte, A-23, 50820, (Zaragoza) Spain";
    if(!strncmp(szMCode, "ATS", 3)) return "Atlas Sayaç Sanayi A. ., Erciyes Teknopark 4.Bina Talas, Kayseri, Turkey";
    if(!strncmp(szMCode, "AUX", 3)) return "Ningbo Sanxing Electric Co., Ltd., No.1166, Mingguang North Rd. Jiangshan Town, Ningbo, China";
    if(!strncmp(szMCode, "AXI", 3)) return "UAB 'Axis Industries', LT-47190, Lithuania";
    if(!strncmp(szMCode, "AZE", 3)) return "AZEL Electronics, B. Ankara, Turkey";
    if(!strncmp(szMCode, "BAM", 3)) return "Bachmann GmbH & Co KG, Ernsthaldenstr, 33 70565, Stuttgart, Germany";
    if(!strncmp(szMCode, "BAR", 3)) return "Baer Industrie-Elektronik GmbH, Fuerth, Germany";
    if(!strncmp(szMCode, "BAS", 3)) return "BASIC INTELLIGENCE TECHNOLOGY CO.,LTD., 1st Floor, No.1 NanLi Rd. PanYu District, GuangZhou, GuangDong, China.";
    if(!strncmp(szMCode, "BBS", 3)) return "BBS Electronics, Singapore";
    if(!strncmp(szMCode, "BCE", 3)) return "ShenZhen B.C Electronic CO.Ltd, 4F, Strength Building,GaoXin Ave.1.s, South, Hi-technology industry Zone, ShenZhen, China";
    if(!strncmp(szMCode, "BEE", 3)) return "Bentec Electricals & Electronics Pvt Ltd, 150,Upen Banerjee Road,Kolkata, Kolkata, India";
    if(!strncmp(szMCode, "BEF", 3)) return "BEFEGA GmbH, Reichenbacher Str. 22, Schwabach, Germany";
    if(!strncmp(szMCode, "BEG", 3)) return "begcomm Communication AB, Brunnehagen 109, GöteborgSweden";
    if(!strncmp(szMCode, "BER", 3)) return "Bernina Electronic AG";
    if(!strncmp(szMCode, "BHG", 3)) return "Brunata A/S, DK-2730 Herlev, Denmark";
    if(!strncmp(szMCode, "BKB", 3)) return "Boendekomfort AB, Box 37, 260 40 Viken, SWEDEN";
    if(!strncmp(szMCode, "BKO", 3)) return "Beko Elektronik AS., Istanbul, Turkey";
    if(!strncmp(szMCode, "BKT", 3)) return "Bekto Precisa, Ibrahima Popovi a bb., Gora de, Bosnia and Herzegovina";
    if(!strncmp(szMCode, "BLU", 3)) return "Bluering, Brescia, Italy";
    if(!strncmp(szMCode, "BME", 3)) return "Beifeng GmbH, 60599 Frankfurt am Main, Germany";
    if(!strncmp(szMCode, "BMI", 3)) return "Badger Meter Inc., 6116 E 15th St., Tulsa, USA";
    if(!strncmp(szMCode, "BMP", 3)) return "BMETERS Polska Sp.z.o.o., Glowna 60, Psary, Poland";
    if(!strncmp(szMCode, "BMT", 3)) return "BMETERS srl, Via Friuli, 3, 33050, Gonars (UD), ITALY";
    if(!strncmp(szMCode, "BRA", 3)) return "Brandes GmbH, D-23701 Eutin, Germany";
    if(!strncmp(szMCode, "BSC", 3)) return "Sanayeh Sanjesh Energy Behineh Sazan Toos, Toos Industrial Estate,Mashhad,Iran";
    if(!strncmp(szMCode, "BSE", 3)) return "Basari Elektronik A.S. Turkey";
    if(!strncmp(szMCode, "BSM", 3)) return "Bluestar Electrical Meter Research Institute, Nanjing, China";
    if(!strncmp(szMCode, "BSP", 3)) return "Byucksan Power Co. Ltd., 6th Fl. New Hosung Bldg. Yoido-Dong, Youngdeungpo-Gu, Seoul, Korea.";
    if(!strncmp(szMCode, "BST", 3)) return "BESTAS Elektronik Optik, Turkey";
    if(!strncmp(szMCode, "BSX", 3)) return "BS-Messtechnik UG, Kassel, Germany";
    if(!strncmp(szMCode, "BTL", 3)) return "BIT-LAB";
    if(!strncmp(szMCode, "BTR", 3)) return "RIA-BTR Produktions-GmbH, D-78176 Blumberg";
    if(!strncmp(szMCode, "BTS", 3)) return "Basari Teknolojik Sistemler AS, Ankara, Turkey";
    if(!strncmp(szMCode, "BUR", 3)) return "Bopp und Reuther Messtechnik GmbH, Speyer, Germany";
    if(!strncmp(szMCode, "BXC", 3)) return "Beijing Fuxing Xiao-Cheng Electronic Technology Stock Co., Ltd, Room 503, Block D, IFEC Blog, No.87 Xisanhuan Beilu, Haidian District, Beijing, China";
    if(!strncmp(szMCode, "BYD", 3)) return "BYD Company Limited, BYD Road NO.3009 PingShan, ShenZhen, China";
    if(!strncmp(szMCode, "BYL", 3)) return "BAYLAN, Turkey";
    if(!strncmp(szMCode, "BZR", 3)) return "Gebr. Bauer GbR, 87719 Mindelheim, Germany";
    if(!strncmp(szMCode, "CAH", 3)) return "MAEC GROUPE CAHORS, ZI DE REGOURD BP 149, 46003 CAHORS CEDEX 9, FRANCE";
    if(!strncmp(szMCode, "CAT", 3)) return "Cubes And Tubes OY, Olli Kytölän tie 1, MUURAME, FINLAND";
    if(!strncmp(szMCode, "CBI", 3)) return "Circuit Breaker Industries, South Africa";
    if(!strncmp(szMCode, "CDL", 3)) return "Customised Data Ltd., 44 Allerburn Lea, Alnwick, UK";
    if(!strncmp(szMCode, "CEB", 3)) return "Cebyc AS, Vestre Rosten 81 / 13th Floor, Tiller, Norway";
    if(!strncmp(szMCode, "CET", 3)) return "Cetinkaya Aydinlatma, Istanbul, Turkey";
    if(!strncmp(szMCode, "CGC", 3)) return "Contor Group S.A., Romania - 310059 Arad, Calea Bodrogului nr. 2-4";
    if(!strncmp(szMCode, "CIR", 3)) return "Circutor, Viladecavalls/Barcelona, Spain";
    if(!strncmp(szMCode, "CLE", 3)) return "Shen Zhen Clou Electronics Co Ltd, Guangdong, China";
    if(!strncmp(szMCode, "CLE", 3)) return "Shen Zhen Clou Electronics Co Ltd, Guangdong, China";
    if(!strncmp(szMCode, "CLO", 3)) return "Clorius Raab Karcher Energi Service A/S";
    if(!strncmp(szMCode, "CLY", 3)) return "Clayster, FO Petersons gata 6, 421 31 Västra Frölunda, Sweden";
    if(!strncmp(szMCode, "CMC", 3)) return "CMC EKOCON d.o.o., IOC ZAPOLJE I/10, LOGATEC, SLOVENIA";
    if(!strncmp(szMCode, "CMP", 3)) return "CM Partner Inc, Yongin, South Korea";
    if(!strncmp(szMCode, "CMV", 3)) return "Comverge, Inc., 5390 Triangle Parkway, Suite 300, Norcross, GA 30041, USA";
    if(!strncmp(szMCode, "CNM", 3)) return "COSTEL, #462-870 COSTEL Bldg., 223-39, Sangdaewon-Dong, Jungwon-Gu, Sungnam-Si, Kyunggi-Do, Korea";
    if(!strncmp(szMCode, "COM", 3)) return "COMMON S.A., Aleksandrowska 67/93, LODZ, POLAND";
    if(!strncmp(szMCode, "CON", 3)) return "Conlog";
    if(!strncmp(szMCode, "CPL", 3)) return "CPL CONCORDIA Soc.Coop., Via A. Grandi, 39 - 41033 Concordia s/S (MO), Italy";
    if(!strncmp(szMCode, "CPO", 3)) return "C3PO S.A., Alejandro Goicoechea 6, Sant Just Desvern, Spain";
    if(!strncmp(szMCode, "CRW", 3)) return "Xuzhou Runwu Science and Technology Development Co. Ltd., NO.5,Huijin Road, Damiao Industry Park,Economic Development Zone, Xuzhou,Jiangsu, P.R.China";
    if(!strncmp(szMCode, "CTR", 3)) return "Contar Electronica Industrial, Lisboa, Portugal";
    if(!strncmp(szMCode, "CTX", 3)) return "Contronix GmbH, Nizzastr 6, Radebeul, Germany";
    if(!strncmp(szMCode, "CUC", 3)) return "cuculus GmbH, Ehrenbergstrasse 11, D-98693 Ilmenau,Germany";
    if(!strncmp(szMCode, "CUR", 3)) return "CURRENT Group, LLC, 20420 Century Boulevard, Germantown, MD, USA";
    if(!strncmp(szMCode, "CWI", 3)) return "Cewe Instrument AB, Nykoping, Sweden";
    if(!strncmp(szMCode, "CWV", 3)) return "CMEC Electric Import & Export Co. Ltd, Beijing 100055, China";
    if(!strncmp(szMCode, "CYE", 3)) return "Quanzhou Chiyoung Electronics Technology Co. Ltd., #20 Hongshan Rd,Shudou community, Changtai St, Quanzhou City, Fujian 362000, China";
    if(!strncmp(szMCode, "CYI", 3)) return "QUANZHOU CHIYOUNG INSTRUMENT CO., LTD, #20 Hongshan Rd,Shudou community, Changtai St, Quanzhou City, Fujian, China 362000";
    if(!strncmp(szMCode, "CYN", 3)) return "Cynox, Weinart Engineering, Bad Zwischenahn, Germany";
    if(!strncmp(szMCode, "CZA", 3)) return "Contazara, Zaragoza, Spain";
    if(!strncmp(szMCode, "CZM", 3)) return "Cazzaniga S.p.A.";
    if(!strncmp(szMCode, "DAE", 3)) return "DAE Srl, Via Trieste, 4/E, Santa Lucia di Piave, Italy";
    if(!strncmp(szMCode, "DAF", 3)) return "Daf Enerji Sanayi ve Ticaret A.S, Atasehir Bulvari Ata Carsi Kat:4 No:52 34758 Atasehir, Istanbul, Turkey";
    if(!strncmp(szMCode, "DAN", 3)) return "Danubia";
    if(!strncmp(szMCode, "DBE", 3)) return "Decibels Electronics P Ltd, Decibels Electronics Pvt Ltd., 6-1-85/4, Saifabad, Hyderabad, AP, India";
    if(!strncmp(szMCode, "DDE", 3)) return "D&D Elettronica srl, Via XXV Aprile, 37, Bresso (MI), ITALY";
    if(!strncmp(szMCode, "DEC", 3)) return "DECODE d.o.o. Data Communications, Belgrade";
    if(!strncmp(szMCode, "DEL", 3)) return "DELTAMESS DWWF GmbH, Sebenter Weg 42, 23758 Oldenburg in Holstein, Germany";
    if(!strncmp(szMCode, "DES", 3)) return "Desi (Alarms) Ltd, Turkey";
    if(!strncmp(szMCode, "DEV", 3)) return "Develco Products, Olof Palmes Allé 40, 8200 Aarhus N, Denmark";
    if(!strncmp(szMCode, "DFE", 3)) return "Dongfang Electronics Co., Ltd., JiChang road 2#, Yantai City, Shandong Province, China";
    if(!strncmp(szMCode, "DFS", 3)) return "Danfoss A/S";
    if(!strncmp(szMCode, "DGC", 3)) return "Digicom S.p.A., Via A.Volta 39, 21010 Cardano al Campo (VA), Italy";
    if(!strncmp(szMCode, "DGM", 3)) return "Diehl Gas Metering GmbH, Industriestrasse 13, Ansbach, Germany";
    if(!strncmp(szMCode, "DIE", 3)) return "Dielen GmbH, Zeppelinstrasse 9, 47638 Straelen, Deutschland";
    if(!strncmp(szMCode, "DIL", 3)) return "DECCAN INFRATECH LIMETED, A3-4/A,Electronic Complex, Kushaiguda, HYDERABAD, INDIA";
    if(!strncmp(szMCode, "DJA", 3)) return "De Jaeger Automation bvba, Molenstraat 200, B-9900 EEKLO, Belgium";
    if(!strncmp(szMCode, "DKY", 3)) return "Electric Power Research Institute of Guangdong Power Grid Corporation, No. 8, Shui jungang Dongfengdong Road, Guangzhou, China";
    if(!strncmp(szMCode, "DMC", 3)) return "DMC International, Al Gharhoud, Dubai, UAE";
    if(!strncmp(szMCode, "DME", 3)) return "DIEHL Metering, Industriestrasse 13, 91522 Ansbach, Germany";
    if(!strncmp(szMCode, "DMP", 3)) return "DM Power Co., Ltd, #SB118 Megavalley, Gwanyang,-Dong, Anyang City, South Korea";
    if(!strncmp(szMCode, "DNO", 3)) return "DENO d.o.o, Zagreb, Croatia";
    if(!strncmp(szMCode, "DNT", 3)) return "Dr Neuhaus Telekommunikation GmbH, Hamburg, Germany";
    if(!strncmp(szMCode, "DNV", 3)) return "DNV KEMA, Utrechtseweg 310, Arnhem, Netherlands";
    if(!strncmp(szMCode, "DPP", 3)) return "DECCAN POWER PRODUCTS PVT. LTD., A3-4/A, Electronic Complex, Kushaiguda, Hyderabad-500062, INDIA";
    if(!strncmp(szMCode, "DRA", 3)) return "Drachmann, Brendstrupgårdsvej 102, DK-8200 Århus N, Denmark";
    if(!strncmp(szMCode, "DRT", 3)) return "DRESSER Italia S.r.l., Via Roma, 772, Talamona (SO), Italy";
    if(!strncmp(szMCode, "DSE", 3)) return "Digitech Systems and Engineering Private Limited, 18 Ramamurthy Street, Nehru Nagar, Chromepet, Chennai-600044, Tamil Nadu, India";
    if(!strncmp(szMCode, "DSE", 3)) return "DSE energy Co., Ltd, 8F, No.531, HsinTien, Taipei, Taiwan";
    if(!strncmp(szMCode, "DWZ", 3)) return "Lorenz GmbH & Co. KG, Burgweg 3, 89601 Schelklingen, Germany";
    if(!strncmp(szMCode, "DZG", 3)) return "Deutsche Zahlergesellschaft";
    if(!strncmp(szMCode, "EAA", 3)) return "Electronic Afzar Azma, Iran";
    if(!strncmp(szMCode, "EAH", 3)) return "Endress+Hauser, 87484 Nesselwang, Germany";
    if(!strncmp(szMCode, "EAS", 3)) return "EAS Elektronik San. Tic. A.S., Ankara, Turkey";
    if(!strncmp(szMCode, "ECH", 3)) return "Echelon Corporation, 550 Meridian Avenue, San Jose, California, USA.";
    if(!strncmp(szMCode, "ECL", 3)) return "Electronics Corporation of India Ltd, Hyderabad, India";
    if(!strncmp(szMCode, "ECS", 3)) return "Herholdt Controls srl, Milan, Italy";
    if(!strncmp(szMCode, "EDI", 3)) return "Enel Distribuzione S.p.A, Via Ombrone, 2, Rome, Italy";
    if(!strncmp(szMCode, "EDM", 3)) return "EDMI Pty. Ltd.";
    if(!strncmp(szMCode, "EEE", 3)) return "3E s.r.l., Via Biandrate, 24, Novara, Italy";
    if(!strncmp(szMCode, "EEO", 3)) return "Eppeltone Engineers, A 293/1 Okhla Industrial Area Phase 1, New Delhi, India";
    if(!strncmp(szMCode, "EFA", 3)) return "EFACEC Engenharia e Sistemas SA, Apartado 3078, MAIA, PORTUGAL";
    if(!strncmp(szMCode, "EFE", 3)) return "Engelmann Sensor GmbH, Rudolf-Diesel-Straße 24-28, 69168 Wiesloch, Germany";
    if(!strncmp(szMCode, "EFR", 3)) return "Europäische Funk-Rundsteuerung, Nymphenburger Strasse 20b, Munich, Germany";
    if(!strncmp(szMCode, "EGA", 3)) return "eGain International AB, Faktorvägen 9, Kungsbacka, Sweden";
    if(!strncmp(szMCode, "EGM", 3)) return "Elgama-Elektronika Ltd, Lithuania";
    if(!strncmp(szMCode, "EHL", 3)) return "Secure Meters Limited";
    if(!strncmp(szMCode, "EIT", 3)) return "EnergyICT NV, 8500 Kortrijk, Belgium";
    if(!strncmp(szMCode, "EKA", 3)) return "Eka Systems, Germantown, MD 20874, USA";
    if(!strncmp(szMCode, "EKT", 3)) return "PA KVANT J.S., Russian Federation";
    if(!strncmp(szMCode, "ELD", 3)) return "Elektromed Elektronik Ltd, Turkey, O.S.B. Uygurlar Cad. No:4 Sincan, Ankara, Turkey";
    if(!strncmp(szMCode, "ELE", 3)) return "Elster Electricity LLC, 208 Rogers Lane, Raleigh, USA";
    if(!strncmp(szMCode, "ELG", 3)) return "Elgas s.r.o., Pardubice, Czech Republic";
    if(!strncmp(szMCode, "ELM", 3)) return "Elektromed Elektronik Ltd, Turkey";
    if(!strncmp(szMCode, "ELO", 3)) return "ELO Sistemas Eletronicos S.A., Brazil";
    if(!strncmp(szMCode, "ELQ", 3)) return "ELEQ b.v., Karl-Ferdinand-Braun-Straße 1, Kerpen, Germay";
    if(!strncmp(szMCode, "ELR", 3)) return "Elster Metering Limited, 130 Camford Way, Luton, UK";
    if(!strncmp(szMCode, "ELS", 3)) return "Elster GmbH, 55252 Mainz-Kastell, Germany";
    if(!strncmp(szMCode, "ELT", 3)) return "ELTAKO GmbH, Hofener Straße 54, 70736 Fellbach, Germany";
    if(!strncmp(szMCode, "ELV", 3)) return "Elvaco AB, Kungsbacka, Sweden";
    if(!strncmp(szMCode, "EMC", 3)) return "Embedded Communication Systems GmbH, vom Staal-Weg 10, 4500 Solothurn, Switzerland";
    if(!strncmp(szMCode, "EME", 3)) return "SC. Electromagnetica SA, Bucharest, Romania";
    if(!strncmp(szMCode, "EMH", 3)) return "EMH metering GmbH & Co. KG (formerly EMH Elektrizitatszahler GmbH & CO KG)";
    if(!strncmp(szMCode, "EML", 3)) return "Emlite ltd, 10 Reynolds Business Park, Stevern Way, PE1 5EL Peterborough, UK";
    if(!strncmp(szMCode, "EMM", 3)) return "Email Metering, Australia";
    if(!strncmp(szMCode, "EMO", 3)) return "Enermet";
    if(!strncmp(szMCode, "EMS", 3)) return "EMS-PATVAG AG, CH-7013 Domat/Ems, Switzerland";
    if(!strncmp(szMCode, "EMT", 3)) return "Elster Messtechnik GmbH, Lampertheim, Germany";
    if(!strncmp(szMCode, "EMU", 3)) return "EMU Elektronik AG, 6432 Rickenbach SZ, Switzerland";
    if(!strncmp(szMCode, "END", 3)) return "ENDYS GmbH";
    if(!strncmp(szMCode, "ENE", 3)) return "ENERDIS, 16 rue Georges Besse SILIC44, 92182 ANTONYFRANCE";
    if(!strncmp(szMCode, "ENG", 3)) return "ENER-G Switch2 Ltd, The Waterfront, Salts Mill Rd, Bradford, BD17 7EZ, UK";
    if(!strncmp(szMCode, "ENI", 3)) return "entec innovations GmbH, Hebelstr. 1, 79379 Müllheim, Germany";
    if(!strncmp(szMCode, "ENL", 3)) return "ENEL d.o.o. Beograd, Belgrade, Serbia and Montenegro ";
    if(!strncmp(szMCode, "ENM", 3)) return "ENMAS GmbH, Holzkoppelweg 23, Kiel, Germany";
    if(!strncmp(szMCode, "ENO", 3)) return "ennovatis GmbH, Stammheimer 10Kornwestheim, Germany";
    if(!strncmp(szMCode, "ENP", 3)) return "Kiev Polytechnical Scientific Research";
    if(!strncmp(szMCode, "ENR", 3)) return "Energisme";
    if(!strncmp(szMCode, "ENS", 3)) return "ENSO NETZ GmbH, Postfach 12 01 23, 01002 Dresden, Dresden, Deutschland";
    if(!strncmp(szMCode, "ENT", 3)) return "ENTES Elektronik, Istambul";
    if(!strncmp(szMCode, "ERE", 3)) return "Enermatics Energy (PTY) LTD, Mertech Building, Glenfield Office Park, Oberon str., Faerleglen, Pretoria, South Africa";
    if(!strncmp(szMCode, "ERI", 3)) return "Easun Reyrolle Limited, 389, Rasukumaki, Hulimavu, Bannerghatta Road, Bangalore-560076, India";
    if(!strncmp(szMCode, "ERL", 3)) return "Erelsan Elektrik ve Elektronik, Turkey";
    if(!strncmp(szMCode, "ESE", 3)) return "ESE Nordic AB, Slottagårdsgatan 9, Vellinge, Sweden";
    if(!strncmp(szMCode, "ESI", 3)) return "Monosan Monofaze Elektrik Motorlari, Turkey";
    if(!strncmp(szMCode, "ESM", 3)) return "Monosan Monofase Elekrik Motorlari, Turkey";
    if(!strncmp(szMCode, "ESO", 3)) return "Monosan Monofaze Elektrik Motorlari, Turkey";
    if(!strncmp(szMCode, "ESS", 3)) return "Energy Saving Systems LTD., Zroshyvalna, 15b, Kiev, Ukraine";
    if(!strncmp(szMCode, "ESY", 3)) return "EasyMeter GmbH";
    if(!strncmp(szMCode, "EUE", 3)) return "E+E Electronic, Langwiesen 7, 4209 Engerwitzdorf, Austria";
    if(!strncmp(szMCode, "EUR", 3)) return "Eurometers Ltd";
    if(!strncmp(szMCode, "EVK", 3)) return "EV KUR ELEKTRIK, Istanbul, Turkey";
    if(!strncmp(szMCode, "EWG", 3)) return "EWG DOO, Bulevar Svetog Cara Konstantina 80-82, Ni , 18106, Serbia";
    if(!strncmp(szMCode, "EWT", 3)) return "Elin Wasserwerkstechnik";
    if(!strncmp(szMCode, "EYT", 3)) return "Enerlyt Potsdam GmbH";
    if(!strncmp(szMCode, "FAN", 3)) return "Fantini Cosmi S.p.A., Via dell'Osio 6, 20090 Caleppio di Settala, Miano, Italy";
    if(!strncmp(szMCode, "FED", 3)) return "Federal Elektrik, Turkey";
    if(!strncmp(szMCode, "FFD", 3)) return "Fast Forward AG, Ruedesheimer Strasse 11, Munich, Germany";
    if(!strncmp(szMCode, "FIM", 3)) return "Frodexim Ltd, Sofia, Bulgaria";
    if(!strncmp(szMCode, "FIN", 3)) return "Finder GmbH, Hans-Böckler-Starße 44, 65468 Trebur-Astheim, Deutschland";
    if(!strncmp(szMCode, "FIO", 3)) return "Pietro Fiorentini, Via Rosellini,1, Milano, Italy";
    if(!strncmp(szMCode, "FLE", 3)) return "XI'AN FLAG ELECTRONIC CO.,LTD, Flag Electronic Industry Park,No.11,Zhangba 6 Rd.(New Zone), Hi-Tech Development Zone, Xi'an, ShaanXi, PRC., China";
    if(!strncmp(szMCode, "FLG", 3)) return "FLOMAG s.r.o, Brno, Czech Republic";
    if(!strncmp(szMCode, "FLO", 3)) return "Flonidan A/S, 8700 Horsens, Denmark";
    if(!strncmp(szMCode, "FLS", 3)) return "FLASH o.s, Istanbul, Turkey";
    if(!strncmp(szMCode, "FMG", 3)) return "Flow Meter Group, Menisstraat 5c, 7091 ZZ Dinxperlo, The Netherlands";
    if(!strncmp(szMCode, "FML", 3)) return "Siemens Measurements Ltd. (Formerly FML Ltd.)";
    if(!strncmp(szMCode, "FNX", 3)) return "Flownetix Ltd, Marlow Bottom, Bucks, UK";
    if(!strncmp(szMCode, "FRE", 3)) return "Frer Srl, Viale Europa, 12, Cologno Monzese (MI), Italy";
    if(!strncmp(szMCode, "FSP", 3)) return "Finmek Space S.p.A., I-34012 Trieste";
    if(!strncmp(szMCode, "FTL", 3)) return "Tritschler GmbH, Schönaustr. 10+12, Laufenburg, Deutschland";
    if(!strncmp(szMCode, "FUS", 3)) return "Fuccesso, 98 Yingchundong, Taizhou, China";
    if(!strncmp(szMCode, "FUT", 3)) return "first:utility, Tachbrook Park, Warwick, UK";
    if(!strncmp(szMCode, "FWS", 3)) return "FW Systeme GmBH, Ehnkenweg 11, 26125 Oldenburg, Germany";
    if(!strncmp(szMCode, "GAV", 3)) return "Carlo Gavazzi Controls S.p.A, Via Safforze 8 C.A.P. 32100, Belluno, Italy";
    if(!strncmp(szMCode, "GBJ", 3)) return "Grundfoss A/S";
    if(!strncmp(szMCode, "GCE", 3)) return "Genergica, Caracas, Venezuela";
    if(!strncmp(szMCode, "GEC", 3)) return "GEC Meters Ltd.";
    if(!strncmp(szMCode, "GEE", 3)) return "GE Energy, Lauder House, Almondvale Business Park, Livingston, UK";
    if(!strncmp(szMCode, "GEL", 3)) return "Industrial Technology Research Institute, Rm. 809, Blg.51, No. 195, Sec. 4, Chung Hsing Rd., Chutung, Hsinchu, Taiwan";
    if(!strncmp(szMCode, "GEN", 3)) return "Goerlitz AG, Germany";
    if(!strncmp(szMCode, "GEO", 3)) return "Green Energy Options Limited, 3 St. Marys Court, Main Street Hardwick, Camridge, England, CB23 7QS";
    if(!strncmp(szMCode, "GET", 3)) return "Genus Electrotech Ltd., Survey No-43,Galpadar Road, Taluka anjar, District-kutch, gandhidham-370110 Gujrat, Taluka anjar,India";
    if(!strncmp(szMCode, "GFM", 3)) return "GE Fuji Meter Co.,Ltd., Horigane Karasugawa 2191, Azumino-City Nagano, Japan";
    if(!strncmp(szMCode, "GIL", 3)) return "Genus Innovations Limited, SPL-2B, RIICO Industrial Area, Sitapura, Jaipur, India";
    if(!strncmp(szMCode, "GIN", 3)) return "Gineers Ltd, 1756 Sofia, Bulgaria";
    if(!strncmp(szMCode, "GMC", 3)) return "GMC-I Messtechnik GmbH, Südwestpark 15, D-90449 Germany";
    if(!strncmp(szMCode, "GME", 3)) return "Global Metering Electronics, Amsterdam, Netherlands";
    if(!strncmp(szMCode, "GMM", 3)) return "Gamma International Egypt, Abour, St 130, industrial area, Cairo, Egypt";
    if(!strncmp(szMCode, "GMT", 3)) return "GMT GmbH, Odenwaldstraße 19, 64521 Groß-Gerau, Germany";
    if(!strncmp(szMCode, "GOE", 3)) return "Genus Overseas Electronics Ltd, Jaipur, India";
    if(!strncmp(szMCode, "GRE", 3)) return "GE2, Green Energy Electronics, R. Fonte Caspolina, N.6,2.C, 2774-521, PACO DE ARCOS, Portugal.";
    if(!strncmp(szMCode, "GRI", 3)) return "Grinpal Energy Management, 50 Oak Avenue, Pretoria, South Africa";
    if(!strncmp(szMCode, "GSP", 3)) return "Ingenieurbuero Gasperowicz";
    if(!strncmp(szMCode, "GST", 3)) return "Shenzhen Golden Square Technology Co.,Ltd, Zone C&D,5/F,Block A3,Shenzhen Digital Technology Park,Hi-Tech South 7 Rd.,Nanshan ,Shenzhen,Guangdong, China";
    if(!strncmp(szMCode, "GTE", 3)) return "GREATech GmbH, Lindenstrasse 66a, 45478 Muelheim an der Ruhr, Germany";
    if(!strncmp(szMCode, "GUH", 3)) return "ShenZhen GuangNing Industrial CO.,Ltd, Room 802, 8th Floor, ShenZhen Software Building ,NanShan, District,ShenZhen ,China";
    if(!strncmp(szMCode, "GWF", 3)) return "Gas- u. Wassermesserfabrik Luzern";
    if(!strncmp(szMCode, "HAG", 3)) return "Hager Electro GmbH, 66131 Saarbruecken, Germany";
    if(!strncmp(szMCode, "HCE", 3)) return "Hsiang Cheng Electric Corp, Hsin-Tien City, Taipei, R.o.China";
    if(!strncmp(szMCode, "HEG", 3)) return "Hamburger Elektronik Gesellschaft";
    if(!strncmp(szMCode, "HEI", 3)) return "Hydro-Eco-Invest SP. Z O.O., Gliwice, Poland";
    if(!strncmp(szMCode, "HEL", 3)) return "Heliowatt";
    if(!strncmp(szMCode, "HER", 3)) return "Hermes Systems, Australia";
    if(!strncmp(szMCode, "HFR", 3)) return "SAERI HEAT METERING TECHNOLOGY CO.,LTD, WANLIAN ROAD 1,SHENHE DISTRICT SHENYANG CHINA";
    if(!strncmp(szMCode, "HIE", 3)) return "Shenzhen Holley South Electronics Technology Co., Ltd., 7/F, No.2 Jianxing Building, Chaguang Industrial Zone, Nanshan District, Shenzhen, China";
    if(!strncmp(szMCode, "HLY", 3)) return "Holley Metering Ltd";
    if(!strncmp(szMCode, "HMI", 3)) return "HMI Energy Co., Ltd, No.38, Alley 175, Lane 75, Sec3, Kongning Rd., Neihu, Taipe, Taiwan";
    if(!strncmp(szMCode, "HMS", 3)) return "Hermes Systems, Australia";
    if(!strncmp(szMCode, "HMU", 3)) return "Hugo Müller GmbH & Co KG, Sturmbühlstraße 145-149, 78054 VS-Schwenningen, Germany";
    if(!strncmp(szMCode, "HND", 3)) return "Shenzhen Haoningda Meters Co., Ltd., 6/F, Huake Mansion, East Science Park, Qiaoxiang Rd, Nanshan District, Shenzhen, China";
    if(!strncmp(szMCode, "HOE", 3)) return "HOENTZSCH GMBH, Gottlieb-Daimler-Str.37, 71334 Waiblingen, Germany";
    if(!strncmp(szMCode, "HOL", 3)) return "Holosys d.o.o, Matije Gupca 7, 49243, Oroslavje, Croatia";
    if(!strncmp(szMCode, "HON", 3)) return "Honeywell Technologies Sarl, Ecublens, Switzerland";
    if(!strncmp(szMCode, "HPL", 3)) return "HPL-Socomec Pvt. Ltd., 133 Pace City 1, Sector 37, Gurgaon, India";
    if(!strncmp(szMCode, "HRM", 3)) return "Hefei Runa Metering Co., Ltd, 1102# jinchi Rd. Luyang industrial park, Hefei, Anhui Province, Hefei, CHINA";
    if(!strncmp(szMCode, "HRS", 3)) return "HomeRider SA, France";
    if(!strncmp(szMCode, "HSD", 3)) return "Ningbo Histar Meter Technology Co.,Ltd., No.181 Haichuan Road Jiangbei District, Ningbo City, Zhejiang Province, CHINA";
    if(!strncmp(szMCode, "HST", 3)) return "HST Equipamentos Electronicos Ltda";
    if(!strncmp(szMCode, "HTC", 3)) return "Horstmann Timers and Controls Ltd.";
    if(!strncmp(szMCode, "HTL", 3)) return "Ernst Heitland GmbH & Co. KG, Erlenstr. 8-10, 42697 Solingen, Deutschland";
    if(!strncmp(szMCode, "HTS", 3)) return "HTS-Elektronik GmbH";
    if(!strncmp(szMCode, "HWT", 3)) return "Huawei Technologies Co. Ltd., Department of Industry Standards, Huawei Industrial Base, Shenzhen, China";
    if(!strncmp(szMCode, "HXD", 3)) return "Beijing HongHaoXingDa Meters CO.,LTD, HouXing,the third street,18,HuoXian, TongZhou., Beijing, China(P.R.C)";
    if(!strncmp(szMCode, "HXE", 3)) return "Hexing Electrical Co., Ltd, Hangzhou, China";
    if(!strncmp(szMCode, "HXW", 3)) return "Hangzhou Xili Watthour Meter Manufacture Co. Ltd., No. 14, JiaQi Road, XianLin Industrial Park, Yuhang District, Hangzhou, China";
    if(!strncmp(szMCode, "HYD", 3)) return "Hydrometer GmbH";
    if(!strncmp(szMCode, "HYE", 3)) return "Zhejiang Hyayi Electronic Industry Co Ltd, Zhejiang, China";
    if(!strncmp(szMCode, "HYG", 3)) return "Hydrometer Group, 91522 Ansbach, Germany";
    if(!strncmp(szMCode, "HZC", 3)) return "TANGSHAN HUIZHONG INSTRUMENTATION CO., LTD., Qinghua Road,New and Hi-Tech Development, Zone,Tangshan,Hebei Province,China, Tangshan, China";
    if(!strncmp(szMCode, "HZI", 3)) return "TANGSHAN HUIZHONG INSTRUMENTATION CO., LTD., Qinghua Road,New and Hi-Tech Development, Zone,Tangshan,Hebei Province,China, Tangshan, China";
    if(!strncmp(szMCode, "ICM", 3)) return "Intracom, Greece";
    if(!strncmp(szMCode, "ICP", 3)) return "PT Indonesia Comnets Plus, PLN building 9th Floor Jl.Jendral Gatot Subroto kav.18, Jakarta Selatan, Indonesia";
    if(!strncmp(szMCode, "ICS", 3)) return "ICSA (India) Limited, Plot No. 12, 1st Floor, Software units Layout, Cyberabad, Hyderabad, India";
    if(!strncmp(szMCode, "ICT", 3)) return "International Control Metering-Technologies GmbHWillhoop 7, D-22453 Hamburg, Germany";
    if(!strncmp(szMCode, "IDE", 3)) return "IMIT S.p.A";
    if(!strncmp(szMCode, "IEC", 3)) return "leonnardo Corporation, Peremogy, 31, Sutysky, Ukraine";
    if(!strncmp(szMCode, "IEE", 3)) return "I.E. Electromatic, S.L, Quart de Poblet (Valencia), Spain";
    if(!strncmp(szMCode, "IFX", 3)) return "Infineon Technologies, AM Campeon 1-12, Nuebiberg, Germany";
    if(!strncmp(szMCode, "IHM", 3)) return "Shenzhen Inhemeter Co., Ltd., 7/F, Science & Industry Park Building, Science & Industry Park, Nanshan District, Shenzhen, China";
    if(!strncmp(szMCode, "IJE", 3)) return "ILJIN Electric, Kyunggi-Do, Korea";
    if(!strncmp(szMCode, "IMS", 3)) return "IMST GmbH, Carl-Friedrich-Gauss-Straße 2-4, 47475 Kamp-Lintfort, Germany";
    if(!strncmp(szMCode, "INC", 3)) return "Incotex, 16th Parkovaya st, 26, Moscow, Russia";
    if(!strncmp(szMCode, "IND", 3)) return "INDRA SISTEMAS, Avda. Bruselas, 35, Alcobendas (Madrid)";
    if(!strncmp(szMCode, "INE", 3)) return "INNOTAS Elektronik GmbH, Rathenaustr. 18a, 02763 Zittau, Germany";
    if(!strncmp(szMCode, "INF", 3)) return "Infometric, Sollentunavägen 50, 19140 Sollentuna, Sweden";
    if(!strncmp(szMCode, "INI", 3)) return "Altero AB, 21211 Malmoe, Sweden";
    if(!strncmp(szMCode, "INP", 3)) return "INNOTAS Produktions GmbH, Rathenaustr. 18a, 02763 Zittau, Germany";
    if(!strncmp(szMCode, "INS", 3)) return "INSYS MICROELECTRONICS GmbH, Hermann-Köhl-Str. 22, 93049, Regensburg, GERMANY";
    if(!strncmp(szMCode, "INT", 3)) return "Infranet Technologies GmbH, 21079 Hamburg, Germany";
    if(!strncmp(szMCode, "INV", 3)) return "Sensus Metering Systems, Ludwigshafen/Rh, Germany";
    if(!strncmp(szMCode, "INX", 3)) return "Innolex Engineering BV, Molenlei 2A, Akersloot, The Netherlands";
    if(!strncmp(szMCode, "IPD", 3)) return "IPD Industrial Products Australia, Sydney, Australia";
    if(!strncmp(szMCode, "ISI", 3)) return "Akcionarsko Drustvo Insa Industrija Satova, Trscanska 21, Belgrade-Zemun, Serbia";
    if(!strncmp(szMCode, "ISK", 3)) return "Iskraemeco, Slovenia";
    if(!strncmp(szMCode, "ISO", 3)) return "Isoil Industria spa, via F.lli Gracchi n.27, Cinisello Balsamo (Milan), Italy";
    if(!strncmp(szMCode, "IST", 3)) return "Ista";
    if(!strncmp(szMCode, "ITA", 3)) return "iTrona GmbH, CH-6432 Rickenbach SZ, Switzerland";
    if(!strncmp(szMCode, "ITB", 3)) return "ITRON Brazil, rua Fioravante Mancino, 1560, CEP: 13175-575 Sumaré, Brazil";
    if(!strncmp(szMCode, "ITC", 3)) return "INTECH TUNISIE, Rue de Tozeur ZI Hammam Zriba, Zaghouan, Tunisia";
    if(!strncmp(szMCode, "ITE", 3)) return "ITRON (Electricity), 52, Avenu Camille Desmoulin, 92130 Issy les Moulineaux, FRANCE";
    if(!strncmp(szMCode, "ITF", 3)) return "ITF Fröschl GmbH, Hauserbachstrasse 9, 93194 Walderbach, Germany";
    if(!strncmp(szMCode, "ITG", 3)) return "ITRON (Gas), 52, Avenue Camille Desmoulin, 92130 Issy les Moulineaux, FRANCE";
    if(!strncmp(szMCode, "ITH", 3)) return "INTELTEH d.o.o., Bozidara Magovca 87, 10000, Zagreb, Croatia";
    if(!strncmp(szMCode, "ITI", 3)) return "ITRON Asia, EJIP Plot 6B-2, Lemah Abang, Bekasi 17550, Jawa Barat, Indonesia";
    if(!strncmp(szMCode, "ITR", 3)) return "Itron";
    if(!strncmp(szMCode, "ITS", 3)) return "ITRON Australia, Rosberg Road, Wingfield, SA, 5013, Adelaide, Australia";
    if(!strncmp(szMCode, "ITU", 3)) return "ITRON United States, 2111 N Molter Road, Liberty Lake, WA 99019, United States";
    if(!strncmp(szMCode, "ITW", 3)) return "ITRON (Water), 52, Avenue Camille Desmoulin, 92130 Issy les Moulineaux, FRANCE";
    if(!strncmp(szMCode, "ITZ", 3)) return "ITRON South Africa, Tygerberg Office Park, Hendrik, Verwoerd Drive, 7500 Plattekloof, Cape Town, South Africa";
    if(!strncmp(szMCode, "IUS", 3)) return "IUSA SA DE CV, Km 109 Carr Panamericana, Pasteje Jocotitlan Edo. de Mex., Mexico";
    if(!strncmp(szMCode, "IWK", 3)) return "IWK Regler und Kompensatoren GmbH";
    if(!strncmp(szMCode, "IZE", 3)) return "iZenze AB, Slottagårdsgatan 9, 235 35 Vellinge, Sweden";
    if(!strncmp(szMCode, "JAN", 3)) return "Janitza electronics GmbH, Lahnau-Waldgirmes, Germany";
    if(!strncmp(szMCode, "JCE", 3)) return "Janz Contadores de Energia SA, Lisbon, Portugal";
    if(!strncmp(szMCode, "JED", 3)) return "JED Co Ltd, Dongan-Gu, Anyang, Kyunggi-Do, South Korea";
    if(!strncmp(szMCode, "JGF", 3)) return "Janz Contagem e Gestao de Fluidos SA, Lisbon, Portugal";
    if(!strncmp(szMCode, "JHM", 3)) return "Changzhou Jianhu Intelligentize Meter Co.,Ltd., No.11 Lijia Industrial District,Wujin, Changzhou, China";
    if(!strncmp(szMCode, "JMT", 3)) return "JM-TRONIC Sp. z o.o., ul. Wybrze e Ko ciuszkowskie 31/33, 00-379 Warszawa, Poland";
    if(!strncmp(szMCode, "JOY", 3)) return "Zhejiang Joy Electronic Technology Co., Ltd., No. 333 North Chayuan Road, Youchegang Town, Xiuzhou District, Jiaxing, China";
    if(!strncmp(szMCode, "JUM", 3)) return "JUMO GmbH & Co. KG, Herrmann - Muth - Strasse 1, 36039 Fulda, Germany";
    if(!strncmp(szMCode, "KAM", 3)) return "Kamstrup Energi A/S";
    if(!strncmp(szMCode, "KAS", 3)) return "Kamstrup A/S, Industrivej 28, 8660 Skanderborg, Denmark";
    if(!strncmp(szMCode, "KAT", 3)) return "KATHREIN-Werke KG, Anton-Kathrein-Straße 1-3, D-83022 Rosenheim, Germany";
    if(!strncmp(szMCode, "KBN", 3)) return "Alpamis IT Ltd, ANKARA, Turkey";
    if(!strncmp(szMCode, "KEL", 3)) return "KELEMINIC d.o.o., Zagreb, Croatia";
    if(!strncmp(szMCode, "KER", 3)) return "KERMS UG (haftungsbeschränkt), Fontanestraße 39, 15569 Woltersdorf, Germany";
    if(!strncmp(szMCode, "KFM", 3)) return "Shenzhen KAIFA Technology Co, Ltd, Shenzhen, China";
    if(!strncmp(szMCode, "KGE", 3)) return "Guangzhou Keli General Electric Co., Ltd, No1.Sui Hua Nan street, Jiang Nan Da Dao Zhong Road, Guangzhou P.R.China";
    if(!strncmp(szMCode, "KHL", 3)) return "Kohler, Turkey";
    if(!strncmp(szMCode, "KKE", 3)) return "KK-Electronic A/S";
    if(!strncmp(szMCode, "KKE", 3)) return "KK-Electronic A/S";
    if(!strncmp(szMCode, "KMB", 3)) return "Kamstrup A/S, Industrivej 28, Stilling, DK 8660 Skanderborg, Denmark";
    if(!strncmp(szMCode, "KMT", 3)) return "Krohne Messtechnik GmbH, Ludwig-Krohne-Straße, Duisburg,Germany";
    if(!strncmp(szMCode, "KNX", 3)) return "KONNEX-based users (Siemens Regensburg)";
    if(!strncmp(szMCode, "KRO", 3)) return "Kromschroder";
    if(!strncmp(szMCode, "KST", 3)) return "Kundo SystemTechnik GmbH";
    if(!strncmp(szMCode, "KSY", 3)) return "KUNDO SystemTechnik GmbH, St Georgen, Germany";
    if(!strncmp(szMCode, "KTC", 3)) return "Kerman Tablo Co, Tehran, Iran";
    if(!strncmp(szMCode, "LAC", 3)) return "Heinz Lackmann GmbH & Co KG, Harkortstrasse 15, 48163 Münster, Germany";
    if(!strncmp(szMCode, "LAN", 3)) return "Langmatz GmbH, Am Gschwend 10, Garmisch-Partenkirchen, Germany";
    if(!strncmp(szMCode, "LCG", 3)) return "Landis+Gyr Meter & System (Zhuhai) Co., Ltd, No.12 Pingdong 3RD,Nanping Industry Community,Zhuhai 519060, P.R.China";
    if(!strncmp(szMCode, "LCR", 3)) return "ShanDong LiChuang Science and Technology Co.,Ltd, No. 9 Fenghuang Road High-tech District, Laiwu ShandongChina";
    if(!strncmp(szMCode, "LDE", 3)) return "Shenzhen Londian Electrics Co., Ltd, 3/F, Build 107#, 1st Nanyou Industrial Zone, Nanshan District, Shenzhen, China";
    if(!strncmp(szMCode, "LEC", 3)) return "Lectrotek Systems Pvt Ltd, 33 Parvati Industrial Estate, 411009, Pune, India";
    if(!strncmp(szMCode, "LEM", 3)) return "LEM HEME Ltd., UK";
    if(!strncmp(szMCode, "LGB", 3)) return "Landis+Gyr Ltd., UK";
    if(!strncmp(szMCode, "LGD", 3)) return "Landis+Gyr GmbH, Germany";
    if(!strncmp(szMCode, "LGS", 3)) return "Landis+Gyr (Pty) Ltd. South Africa";
    if(!strncmp(szMCode, "LGU", 3)) return "LG Uplus Corp, Namdaemunno 5-ga, Jung-gu, Seoul, Korea";
    if(!strncmp(szMCode, "LGZ", 3)) return "Landis+Gyr AG Zug";
    if(!strncmp(szMCode, "LHA", 3)) return "Atlantic Meters, South Africa";
    if(!strncmp(szMCode, "LML", 3)) return "LUMEL, Poland";
    if(!strncmp(szMCode, "LNT", 3)) return "Larsen & Toubro Ltd, MPS, KHebbal-Hootagalli, Mysore, India";
    if(!strncmp(szMCode, "LSE", 3)) return "Landis & Staefa electronic";
    if(!strncmp(szMCode, "LSK", 3)) return "LS Industrial Systems Co Ltd, Cheongju, South Korea";
    if(!strncmp(szMCode, "LSP", 3)) return "Landis+Gyr GmbH, Germany";
    if(!strncmp(szMCode, "LSZ", 3)) return "Siemens Building Technologies";
    if(!strncmp(szMCode, "LUG", 3)) return "Landis+Gyr GmbH, Germany";
    if(!strncmp(szMCode, "LUN", 3)) return "Protokol Sanayi ve Ticaret, Karacaoglan Mah., 167 Sok., No 42 Isikkent, Izmir, Turkey";
    if(!strncmp(szMCode, "LYE", 3)) return "Jiangsu Linyang Electronics Co.,Ltd., No.666,Linyang Road, Qidong, China";
    if(!strncmp(szMCode, "MAC", 3)) return "RUDNAP Group Meter & Control, Omladinskih brigada 182, Belgrade, Serbia";
    if(!strncmp(szMCode, "MAD", 3)) return "Maddalena S.p.A.. Italy";
    if(!strncmp(szMCode, "MAE", 3)) return "Mates Elektronik Metin Ates, Ankara, Turkey";
    if(!strncmp(szMCode, "MAN", 3)) return "Manthey GmbH, Walter-Freitag-Str. 30, 42897 Remscheid, Deutschland";
    if(!strncmp(szMCode, "MAT", 3)) return "Mitsubishi Electric Automation, Bangkok, Thailand";
    if(!strncmp(szMCode, "MAX", 3)) return "MAXMET Inc, Seogu, Daejeon 302-834, Korea";
    if(!strncmp(szMCode, "MBS", 3)) return "MBS AG, Eisnachstraße 51, 74429 Sulzbach-Laufen, Germany";
    if(!strncmp(szMCode, "MCR", 3)) return "MICRORISC, D lnická 222, Ji ín, Czech Republic";
    if(!strncmp(szMCode, "MDE", 3)) return "Diehl Metering Deutschland, Industriestraße 13, Ansbach, Germany";
    if(!strncmp(szMCode, "MEC", 3)) return "Mitsubishi Electric Corporation, 1-8, Midorimachi Fukuyama-city Hiroshima, 720-8647, Japan";
    if(!strncmp(szMCode, "MED", 3)) return "MAHARASHTRA STATE ELECTRICITY DISTRIBUTION COMPANY LIMITED, PLOT NO. G-9, PRAKASHGAD, 5TH FLOOR, PROF. ANANT KANEKAR MARG, BANDRA (EAST)MUMBAI 400051, INDIA";
    if(!strncmp(szMCode, "MEH", 3)) return "Mueller-electronic GmbH, Fritz-Garbe-Str. 2, 30974, Wennigsen, Germany";
    if(!strncmp(szMCode, "MEI", 3)) return "Sensus Metering Systems, Ludwigshafen/Rh, Germany";
    if(!strncmp(szMCode, "MEL", 3)) return "Mikroelektronika a.d, Banja Luka, Bosnia and Herzegovina";
    if(!strncmp(szMCode, "MEM", 3)) return "MEMS AG, Segelhofstrasse, CH-5405 Baden-Dättwil, Switzerland";
    if(!strncmp(szMCode, "MET", 3)) return "METRA Energie-Messtechnik GmbH, Speyer, Germany";
    if(!strncmp(szMCode, "MIC", 3)) return "Microtronics Engineering GmbH, Hauptstrasse 7, A-3244 Ruprechtshofen, Austria";
    if(!strncmp(szMCode, "MII", 3)) return "Miitors ApS, Miitors ApS, VBI Park, Chr. M Østergaardsvej 4aDK-8700 Horsens, Denmark";
    if(!strncmp(szMCode, "MIM", 3)) return "Malaysian Intelligence Meters Sdn. Bhd., No. 3, Jalan Pemberita U1/49, Temasya Industrial Park, Seksyen U1, Glenmarie Shah Alam, Selangor Darul Ehsan, Malaysia";
    if(!strncmp(szMCode, "MIR", 3)) return "MIR Research and Production Association, 51 Uspeshnaya644105 Omsk, Russia";
    if(!strncmp(szMCode, "MIS", 3)) return "Iskra MIS d.d, 4000 Kranj, Slovenia";
    if(!strncmp(szMCode, "MKE", 3)) return "MKEK Genel Mudurlugu Gazi Fisek (ELSA), Ankara, Turkey";
    if(!strncmp(szMCode, "MKL", 3)) return "MAKEL Elektrik Malzemeleri, Turkey";
    if(!strncmp(szMCode, "MKS", 3)) return "MAK-SAY Elektrik Elektronik, Turkey";
    if(!strncmp(szMCode, "MMC", 3)) return "Modern Meters Co., Damascus Sahnaya, Syria,";
    if(!strncmp(szMCode, "MMI", 3)) return "MyMeterInfo, 95 rue du Morellon, 38070 Saint Quentin, Fallavier, France";
    if(!strncmp(szMCode, "MMS", 3)) return "Brunswick Bowling and Billiards UK Ltd, Unit L1, Temple Court, Knights Park, Knight Road, Strood, Kent, UK";
    if(!strncmp(szMCode, "MNS", 3)) return "MANAS Elektronik, Turkey";
    if(!strncmp(szMCode, "MOS", 3)) return "MOMAS SYSTEMS NIGERIA LIMITED, #4, Bode Thomas Street, Surulere, Lagos, NIGERIA";
    if(!strncmp(szMCode, "MPA", 3)) return "Mega Power Automation International Limited, 16/F., Block A-1, Fortune Factory Building, 40 Lee Chung Street, Chai Wan, Hong Kong.";
    if(!strncmp(szMCode, "MPR", 3)) return "Michael Rac GmbH, Sonnenfeld 29, 91522 Ansbach, Germany";
    if(!strncmp(szMCode, "MPS", 3)) return "Multiprocessor Systems Ltd, Bulgaria";
    if(!strncmp(szMCode, "MRT", 3)) return "MIRTEK LTD., Gagarin street, Building 4, Stavropol, Russia";
    if(!strncmp(szMCode, "MSB", 3)) return "MISA SDN BHD, LOT 30, JALAN MODAL 23/2, 40300, SHAH ALAM, SELANGOR, MALAYSIA";
    if(!strncmp(szMCode, "MSE", 3)) return "Mahashakti Energy Limited, A-8, New Focal Point, Dabwali Road, Bathinda (Punjab), India";
    if(!strncmp(szMCode, "MSM", 3)) return "MS-M Co., Ltd., 237 Bukjung-Dong Yangsan-City, Korea";
    if(!strncmp(szMCode, "MSO", 3)) return "Metiso, Tr anska 21, Zemun, Belgrade, Serbia";
    if(!strncmp(szMCode, "MSY", 3)) return "MAK-SAY Elektrik Elektronik Malzemeleri, Turkey";
    if(!strncmp(szMCode, "MTC", 3)) return "Metering Technology Corporation, USA";
    if(!strncmp(szMCode, "MTH", 3)) return "njmeter, Binjiang Development Zone of Jiangning Road No. 6Nanjing, China";
    if(!strncmp(szMCode, "MTI", 3)) return "Micrtotech Industries Pakistan, Plot#2,Street#2,Attari industrial estate 18-Km. Ferozepure Raod, Lahore, Pakistan";
    if(!strncmp(szMCode, "MTM", 3)) return "Metrum Sweden AB, Vestagatan 2A, Gothenburg, Sweden";
    if(!strncmp(szMCode, "MTS", 3)) return "MeteRSit S.r.L., Viale dell'Industria, 31, Padova, Italy";
    if(!strncmp(szMCode, "MTX", 3)) return "Matrix Energy Pvt. Ltd., Soni Arcade, No. 242, 2nd Floor, 7th Cross, 6th Block, Banashankari 3rd Stage, Bangalore, INDIA";
    if(!strncmp(szMCode, "MUK", 3)) return "Meters UK Ltd, Whitegate, White Lund Trading Estate, Lancaster, UK";
    if(!strncmp(szMCode, "MWU", 3)) return "METRONA Wärmemesser Union GmbH, Aidenbachstraße 40, 81379 München, Germany";
    if(!strncmp(szMCode, "MXM", 3)) return "Maxim India Integrated Circuit Design Pvt Ltd., 132/133, Divyasree Technolopolis, Off Airport Road, Bangalore, India";
    if(!strncmp(szMCode, "NAR", 3)) return "NARI Group Corporation-NARI Technology Development Co., Ltd, No.8 NARI Rd. Gulou District, Nanjing, China";
    if(!strncmp(szMCode, "NDF", 3)) return "NÚCLEO DURO FELGUERA, Avda. de la Industria, 24, 28760, Tres Cantos, Madrid";
    if(!strncmp(szMCode, "NDM", 3)) return "Northern Design, 228 Bolton Road, Bradford, United Kingdom";
    if(!strncmp(szMCode, "NES", 3)) return "NORA ELK. MALZ. SAN. ve T C. A. ., nönü Cad. Sümer Sok. Zita Merkezi C1 Blok No:9 Kozyata -Kad köy- STANBUL, TURKEY";
    if(!strncmp(szMCode, "NIS", 3)) return "Nisko Industries Israel";
    if(!strncmp(szMCode, "NJC", 3)) return "NAMJUN Co Ltd, Gimhae Gyoungnam, South Korea";
    if(!strncmp(szMCode, "NMS", 3)) return "Nisko Advanced Metering Solutions Israel";
    if(!strncmp(szMCode, "NNT", 3)) return "2N Telekomunikace a.s., Modranska 621, 14301 Praha 4, Czech Republic";
    if(!strncmp(szMCode, "NRM", 3)) return "Norm Elektronik, Turkey";
    if(!strncmp(szMCode, "NTC", 3)) return "Nuri Telecom Co Ltd, Geumcheon-gu, Seoul, Korea";
    if(!strncmp(szMCode, "NTM", 3)) return "Netinium, Postbus 86, Wormerveer, The Netherlands";
    if(!strncmp(szMCode, "NVN", 3)) return "NOVEN ENERGY AND ICT LTD., Hacettepe University KOSGEB Technology Center T1-Blok B:14 Beytepe, Ankara,Turkiye";
    if(!strncmp(szMCode, "NWM", 3)) return "Ningbo Water Meter Co.Ltd., No.99 Lane 268 Beihai Road Jiangbei District, Ningbo City Zhejiang ProvinceCHINA";
    if(!strncmp(szMCode, "NXP", 3)) return "NXP Semiconductors, High Tech Campus 32, 5656AE Eindhoven, The Netherlands";
    if(!strncmp(szMCode, "NYG", 3)) return "Ningbo Yonggang Instrument Co.,Ltd, Weisan Road, West Industrial Zone, Xinpu Town, Cixi City, China";
    if(!strncmp(szMCode, "NYN", 3)) return "Nanjing Yuneng Instrument Co Ltd, Nanjing, China";
    if(!strncmp(szMCode, "NZR", 3)) return "Nordwestdeutsche Zählerrevision Ing. Aug. Knemeyer GmbH & Co. KG, Heideweg 33, 49196, Bad Laer, Germany";
    if(!strncmp(szMCode, "OAS", 3)) return "Omni Agate Systems, Chennai, India";
    if(!strncmp(szMCode, "ODI", 3)) return "OAS Digital Infrastructures Pvt. Ltd., No:4/3, Stringer Road, Periamet, Vepery, Chennai, INDIA";
    if(!strncmp(szMCode, "OEE", 3)) return "ONUR Elektrik ve Elektronik, Turkey";
    if(!strncmp(szMCode, "OMS", 3)) return "OMNISYSTEM Co., Ltd., Goyang-shi, Gyeonggi-do, Korea";
    if(!strncmp(szMCode, "ONR", 3)) return "ONUR Elektroteknik, Turkey";
    if(!strncmp(szMCode, "ONS", 3)) return "ONUR Elektroteknik, Turkey";
    if(!strncmp(szMCode, "OPT", 3)) return "Optec GmbH, Grundstrasse 22, 8344 Bäretswil, Switzerland";
    if(!strncmp(szMCode, "ORB", 3)) return "ORBIS Tecnologia Electrica, SA, Madrid, Spain";
    if(!strncmp(szMCode, "ORM", 3)) return "Ormazabal, B Basauntz, 2, Igorre, Spain";
    if(!strncmp(szMCode, "OSA", 3)) return "Osaki Electric Co., Ltd.(Europe), Gotanda-Square, Tokyo, Japan";
    if(!strncmp(szMCode, "OSK", 3)) return "Osaki Electric Co Ltd. (Japan), Shinagawa-ku, Tokyo, Japan";
    if(!strncmp(szMCode, "OZK", 3)) return "Oz-kar Enerji, Kayseri, Turkey";
    if(!strncmp(szMCode, "PAD", 3)) return "PadMess GmbH, Germany";
    if(!strncmp(szMCode, "PAF", 3)) return "FAP PAFAL S.A., 26 Lukasinskiego street, widnica, Poland";
    if(!strncmp(szMCode, "PAK", 3)) return "Paktim Energy Sp.zo.o., ul. Swiety Marcin 29/8, 61-806 Poznan, Poland";
    if(!strncmp(szMCode, "PAN", 3)) return "Panasonic Corporation, 800 Tsutsui-cho, Yamatokoriyama-shiNara Pref., Japan";
    if(!strncmp(szMCode, "PDX", 3)) return "Paradox Engineering SA, Via Ronchetto, 9, Cadempino, Switzerland";
    if(!strncmp(szMCode, "PEL", 3)) return "Pak Elektron Ltd. (PEL), 14-km Ferozpur Road, Lahore, Pakistan";
    if(!strncmp(szMCode, "PGP", 3)) return "P.G.P. - Smart Sensing s.a., Rue Fond Cattelain 2 / 1.15, Mont-St-Guibert, Belgium";
    if(!strncmp(szMCode, "PHL", 3)) return "HangZhou PAX Electronic Technology Co., Ltd., China";
    if(!strncmp(szMCode, "PII", 3)) return "PiiGAB Processinformation i Goteborg AB, Sweden";
    if(!strncmp(szMCode, "PIK", 3)) return "pikkerton GmbH, Kienhorststrasse 70, 13403 Berlin, Germany";
    if(!strncmp(szMCode, "PIL", 3)) return "Pilot Systems (London) Ltd, Chiswick, London";
    if(!strncmp(szMCode, "PIM", 3)) return "Power Innovation GmbH, Rehland 2, Achim,Germany";
    if(!strncmp(szMCode, "PIP", 3)) return "Hermann Pipersberg jr., Felder Hof 2, D-42899 Remscheid, Deutschland";
    if(!strncmp(szMCode, "PLO", 3)) return "Weihai Ploumeter Co. Ltd., : No. 28 Hengrui Street, Torch Hi-Tech Industries Development Zone, Weihai, Shandong, China";
    if(!strncmp(szMCode, "PMG", 3)) return "Sensus Metering Systems, Ludwigshafen/Rh, Germany";
    if(!strncmp(szMCode, "POD", 3)) return "PowerOneData, Bangalore 560082, India";
    if(!strncmp(szMCode, "POW", 3)) return "PowerApp, Esromgade 15 opg. 2 - 2 sal., Copenhagen, Denmark";
    if(!strncmp(szMCode, "POZ", 3)) return "ZEUP Pozyton sp. z o.o, ul. Czestochowa, Poland";
    if(!strncmp(szMCode, "PPC", 3)) return "Power Plus Communications AG, Am Exerzierplatz 2, 68167 Mannheim, Germany";
    if(!strncmp(szMCode, "PPS", 3)) return "Palace Power Systems, 50 Oak Avenue, Pretoria, South Africa";
    if(!strncmp(szMCode, "PRE", 3)) return "Predicate Software, 7 Protea Ave, Dooringkloof, 0140, Centurion, Gauteng, South Africa";
    if(!strncmp(szMCode, "PRG", 3)) return "Paud Raad Industrial Group, No. 18, 2nd St., Shah Nazari Ave.,Madar Sq., Mirdamad Blvd., Tehran, Iran";
    if(!strncmp(szMCode, "PRI", 3)) return "Polymeters Response International Ltd.";
    if(!strncmp(szMCode, "PRO", 3)) return "Proton - Elektromed Ltd, Ankara, Turkey";
    if(!strncmp(szMCode, "PST", 3)) return "PSTec Co.,Ltd, Seoul, Korea";
    if(!strncmp(szMCode, "PWB", 3)) return "Paul Wegener GmbH, Ballenstedt, Germany";
    if(!strncmp(szMCode, "PWR", 3)) return "Powrtec, Scotts Valley, CA 95066, USA";
    if(!strncmp(szMCode, "PYU", 3)) return "PYUNGIL Co. Ltd, Anyang-si, Gyeonggi-do, Korea";
    if(!strncmp(szMCode, "QDS", 3)) return "Qundis GmbH, Sondershaeuser Landstrasse 27, Mühlhausen, Germany";
    if(!strncmp(szMCode, "QTS", 3)) return "QT systems ab, Alfavägen 3, 92133 Lycksele, Sweden";
    if(!strncmp(szMCode, "RAC", 3)) return "Michael Rac GmbH, Sonnenfeld 29, Ansbach, Germany";
    if(!strncmp(szMCode, "RAD", 3)) return "Radiocrafts AS, Sandakerveien 64, 0484 OSLO, NORWAY";
    if(!strncmp(szMCode, "RAM", 3)) return "Rossweiner Armaturen und Messgeräte GmbH & Co. OHG, Wehrstraße 8, Roßwein, Germany";
    if(!strncmp(szMCode, "RAS", 3)) return "Rubikon Apskaitos Sistemos, Vilnius, Lithuania";
    if(!strncmp(szMCode, "RCE", 3)) return "RC ENERGY METERING PVT,LTD., B-65 GATE NO.1,N.I.A.PHASE-II NEW DELHI -110028, DELHI, INDIA";
    if(!strncmp(szMCode, "REC", 3)) return "Zhejiang Reallin Electron Co.,Ltd, 2F,Building3,No.202 Zhenzhong Rd,Sandun Technology Park,Xihu District, Hangzhou, China";
    if(!strncmp(szMCode, "REL", 3)) return "Relay GmbH, Germany";
    if(!strncmp(szMCode, "REM", 3)) return "Remote Energy Monitoring, Tring, UK";
    if(!strncmp(szMCode, "RIC", 3)) return "Richa Equipments Pvt. Ltd., Z B 5-6/487, Zulfe Bengal, Dilshad Garden Shahdara, Delhi, India";
    if(!strncmp(szMCode, "RIL", 3)) return "Rikken Instrumentation Limited, Plot No. 369, Phase 2, Industrial Area, Panchkula, India";
    if(!strncmp(szMCode, "RIM", 3)) return "CJSC Radio and Microelectronics, 630082, Novosibirsk, the Red Prospectus, 220, the case 17, Novosibirsk, Russia";
    if(!strncmp(szMCode, "RIT", 3)) return "Ritz Instrument Transformers GmbH, Wandsbeker Zollstr. 92 98, 22041 Hamburg, Germany";
    if(!strncmp(szMCode, "RIZ", 3)) return "RIZ Transmitters, Bozidareviceva 13, Zagreb, Croatia";
    if(!strncmp(szMCode, "RKE", 3)) return "Viterra Energy Services (formerly Raab Karcher ES)";
    if(!strncmp(szMCode, "RML", 3)) return "ROLEX METERS RPIVATE LIMITED, Plot No 20&21, Prashanthi Nagar, Kukatpally Industrial Estate, Hyderabad, India";
    if(!strncmp(szMCode, "RMR", 3)) return "Advanced Technology RAMAR, Christchurch, UK";
    if(!strncmp(szMCode, "RSA", 3)) return "Rahrovan Sepehr Andisheh Pte. Co.";
    if(!strncmp(szMCode, "RSW", 3)) return "RSW Technik GmbH, Giessen, Germany";
    if(!strncmp(szMCode, "SAA", 3)) return "Sanjesh Afzar Asia Ltd. Co., 3 rd Flr/No. 8/16 St./Gandi Ave./Tehran, Iran";
    if(!strncmp(szMCode, "SAC", 3)) return "Sacofgas 1927 SpA, Via Ascanio Sforza 85, Milano, Italy";
    if(!strncmp(szMCode, "SAG", 3)) return "SAGEM, Cergy Saint-Christophe, France";
    if(!strncmp(szMCode, "SAM", 3)) return "Siemens AG Österreich, AMIS (Automated Metering and Information System), Ruthnergasse 3, Vienna, Austria";
    if(!strncmp(szMCode, "SAP", 3)) return "Sappel";
    if(!strncmp(szMCode, "SAT", 3)) return "SATEC Ltd, 7 Hamarpe Street, Jerusalem, Israel";
    if(!strncmp(szMCode, "SBC", 3)) return "Saia-Burgess Controls, Bahnhofstrasse 18, 3280 Murten, Switzerland";
    if(!strncmp(szMCode, "SCE", 3)) return "Seo Chang Electric Communication Co Ltd, Daegu, Korea";
    if(!strncmp(szMCode, "SCH", 3)) return "Schinzel GmbH";
    if(!strncmp(szMCode, "SCW", 3)) return "ScatterWeb GmbH, Charlottenstr. 16, Berlin, Germany";
    if(!strncmp(szMCode, "SDC", 3)) return "SdC Sistemas de Contagem, Vila Nova de Famalicao, PT";
    if(!strncmp(szMCode, "SDM", 3)) return "Shandong Delu Measurement Co., Ltd., Tower C, Qiln Software Park, High-Tech Industrial Development, JiNan, China";
    if(!strncmp(szMCode, "SEC", 3)) return "Schneider Electric Canada, Saanichton, Canada";
    if(!strncmp(szMCode, "SEE", 3)) return "El Sewedy Electrometer Egypt, 6th of October, Egypt";
    if(!strncmp(szMCode, "SEN", 3)) return "Sensus Metering Systems, Ludwigshafen/Rh, Germany";
    if(!strncmp(szMCode, "SGA", 3)) return "smartGAS Mikrosensorik GmbH, Kreuzenstraße 98, 74076 Heilbronn, Germany";
    if(!strncmp(szMCode, "SGM", 3)) return "Swiss Gas Metering AG, Reichenauerstrasse, Domat/Ems, Switzerland";
    if(!strncmp(szMCode, "SHD", 3)) return "Beijing SanHuaDeBao Energy Technoligy Co.,Ltd., Floor4 Jinyanlong R&D Building Jiancaicheng West Road Changping District Beijing City China";
    if(!strncmp(szMCode, "SHE", 3)) return "Shenzhen SingHang Elec-tech Co., Ltd., Rm203-206, Terra Science & Technology Park, Futian District, Shenzhen, China";
    if(!strncmp(szMCode, "SHM", 3)) return "Shanghai Metering, China, No.2065 Kongjiang Road, Shanghai, China";
    if(!strncmp(szMCode, "SIE", 3)) return "Siemens AG";
    if(!strncmp(szMCode, "SIT", 3)) return "SITEL doo, Belgrade, Serbia and Montenegro";
    if(!strncmp(szMCode, "SLB", 3)) return "Schlumberger Industries Ltd.";
    if(!strncmp(szMCode, "SLP", 3)) return "Sylop, ul. Jagiellonska 4, PL-32830 Wojnicz, Poland";
    if(!strncmp(szMCode, "SLX", 3)) return "Ymatron AG, Bruelstrasse 7, Dielsdorf, Switzerland";
    if(!strncmp(szMCode, "SMC", 3)) return "Pending";
    if(!strncmp(szMCode, "SME", 3)) return "Siame, Tunisia";
    if(!strncmp(szMCode, "SMG", 3)) return "Samgas s.r.l., SP 33 km 0,600 20080, Vernate (MI), Italy";
    if(!strncmp(szMCode, "SML", 3)) return "Siemens Measurements Ltd.";
    if(!strncmp(szMCode, "SMT", 3)) return "Smarteh d.o.o., Trg tigrovcev 1, Tolmin, Slovenia";
    if(!strncmp(szMCode, "SNM", 3)) return "ShenZhen Northmeter Co.Ltd , floor 5, Dongshan Building, Huafeng first science park,Baoan, Shenzhen, China";
    if(!strncmp(szMCode, "SNR", 3)) return "NTN-SNR, 1 Rue des Usines, 74010 Annecy, France";
    if(!strncmp(szMCode, "SNS", 3)) return "Signals and Systems India Private Limited, MF-7, Cipet Hostel Road, Thiru-Vi-Ka Industrial Estate, Chennai, India";
    if(!strncmp(szMCode, "SOC", 3)) return "SOCOMEC, 1, rue de Westhouse, 67230 Benfeld, France";
    if(!strncmp(szMCode, "SOF", 3)) return "Softflow.de GmbH, Dorfstasse, 15834 Gross Machnow, Germany";
    if(!strncmp(szMCode, "SOG", 3)) return "Sogecam Industrial, S.A., C/ Rosalind Franklin, 22-24, Campanillas (Málaga), Spain";
    if(!strncmp(szMCode, "SOL", 3)) return "Soledia Srl, Via di Selva Candida 85, Rome, Italy";
    if(!strncmp(szMCode, "SOM", 3)) return "Somesca, 80 rue Jean Jaures, 92270 Boois colombes, France";
    if(!strncmp(szMCode, "SON", 3)) return "Sontex SA";
    if(!strncmp(szMCode, "SPL", 3)) return "Sappel";
    if(!strncmp(szMCode, "SPX", 3)) return "Sensus Metering Systems, Ludwigshafen/Rh, Germany";
    if(!strncmp(szMCode, "SRE", 3)) return "Guangzhou Sunrise Electronics Development Co., Ltd., Guangzhou Avenue South, Guangzhou, Guangdong, China";
    if(!strncmp(szMCode, "SRF", 3)) return "Saraf Industries, Saraf Industries, Bathinda Road, Rampura Phul - 151103, Punjab, India";
    if(!strncmp(szMCode, "SRN", 3)) return "Shandong SARON Intelligent Technology Co., Ltd., 3F,South E,International Business Center, Environmental Technology Area, Middle of Zhengfeng Road, Jinan City Shandong Province,China";
    if(!strncmp(szMCode, "SRV", 3)) return "Servic LLC, Kirova 16-9, Dniprodzerzhinsk, Ukraine";
    if(!strncmp(szMCode, "SSN", 3)) return "Silver Spring Networks, 555 Broadway Street, Redwood City, United States";
    if(!strncmp(szMCode, "SST", 3)) return "Qingdao Haina Electric Automation Systems Co., Ltd., No.151,Zhuzhou Road, Laoshan, Qingdao, China";
    if(!strncmp(szMCode, "STA", 3)) return "Shenzhen Star Instrument Co Ltd, Shenzhen, China";
    if(!strncmp(szMCode, "STC", 3)) return "Sunrise Technology Co., Ltd, Building C, Xiyuan 8th Road 2#, West-Lake Technological & Economic Zone, Hangzhou, China";
    if(!strncmp(szMCode, "STD", 3)) return "Stedin, Essebaan 71, Capelle a/d Ijssel, Netherlands";
    if(!strncmp(szMCode, "STR", 3)) return "Strike Technologies, South Africa";
    if(!strncmp(szMCode, "STV", 3)) return "STV Automation, Branch of STV Electronic, Detmold, Germ.";
    if(!strncmp(szMCode, "STZ", 3)) return "Steinbeis Innovation Center Embedded Design and Networking c/oUniversity of Cooperative Education Loerrach Hangstrasse 46-50, D79539 Loerrach";
    if(!strncmp(szMCode, "SVM", 3)) return "AB Svensk Värmemätning SVM";
    if(!strncmp(szMCode, "SWI", 3)) return "Swistec GmbH, Graue-Burg-Strasse 24-26, Bornheim, 53332 Germany";
    if(!strncmp(szMCode, "SWT", 3)) return "Beijing Swirling Technology Co. Ltd, Beijing, China";
    if(!strncmp(szMCode, "SYN", 3)) return "SMSISTEM Ltd., ANKARA, Turkey";
    if(!strncmp(szMCode, "TAG", 3)) return "Telma AG, Gewerbeweg 10, 3662 Seftigen, Switzerland";
    if(!strncmp(szMCode, "TAT", 3)) return "Tatung Co., 22, Chungshan N. Rd., 3rd Sec., Taipei, Taiwan";
    if(!strncmp(szMCode, "TAY", 3)) return "Taytech Otomasyon ve Bilisim Teknolojileri LTD. Sti, Tasdelen Gungoren Mah. Izan Sok. No:15 Cekmekoy., Istanbul, Turkey";
    if(!strncmp(szMCode, "TCE", 3)) return "Qindao Techen Electronic Technology Co.,LTD, No. 169 Songling Road. Laoshan District, Qingdao, China";
    if(!strncmp(szMCode, "TCH", 3)) return "Techem Service AG & Co. KG";
    if(!strncmp(szMCode, "TDC", 3)) return "Telecom Design, Rue Romaine Voie de Remora, GradignanFRANCE";
    if(!strncmp(szMCode, "TEC", 3)) return "TECSON Digital, Felde, Germany";
    if(!strncmp(szMCode, "TEP", 3)) return "TEPEECAL, 69730 Genay, France";
    if(!strncmp(szMCode, "TFC", 3)) return "Toos Fuse Co. 375 Sanat Blvd. Toos Industrial Estate, Mashad, Iran";
    if(!strncmp(szMCode, "THE", 3)) return "Theben AG, Hohenbergstrasse 32, 72401 Haigerloch, Germany";
    if(!strncmp(szMCode, "TIP", 3)) return "TIP GmbH, Bahnhofstr. 26, 99842 Ruhla, Germany";
    if(!strncmp(szMCode, "TIX", 3)) return "Tixi.Com GmbH, D-13465 Berlin";
    if(!strncmp(szMCode, "TLM", 3)) return "Theodor Lange Messgeräte GmbH, Rodeberg 7, 31226 Peine,Germany";
    if(!strncmp(szMCode, "TMK", 3)) return "Timi Kosova Sh.p.k.";
    if(!strncmp(szMCode, "TMS", 3)) return "TEMASS IMALAT A.S, Macunkoy, Ankara, Turkey";
    if(!strncmp(szMCode, "TPL", 3)) return "Teplocom Holding, 45, Vyborgskaya Naberezhnaya, ST Petersburg, Russian Federation";
    if(!strncmp(szMCode, "TRI", 3)) return "Tritech Technology AB, Sturegatan, 10-12 PO Box 1094, SE-172 22 Sundbyberg, Stockholm, Sweden";
    if(!strncmp(szMCode, "TRJ", 3)) return "SHENZHEN TECHRISE ELECTRONICS CO.,LTD, Building 112,1st Industrial park, Liantang, Luohu District, Shenzhen City, China.";
    if(!strncmp(szMCode, "TRL", 3)) return "Trilliant Inc., 610 du Luxembourg, Granby, (Quebec), Canada, J2J 2V2";
    if(!strncmp(szMCode, "TRV", 3)) return "Transvaro Elektron Aletleri A.S., Turkey";
    if(!strncmp(szMCode, "TSD", 3)) return "Theobroma Systems Design und Consulting GmbH, Gutheil-Schoder Gasse 17, Wien, Austria";
    if(!strncmp(szMCode, "TTM", 3)) return "Toshiba Toko Meter Systems Co., Ltd., 12-7, Shiba 1-chome, Minato-ku, TOKYO, JAPAN";
    if(!strncmp(szMCode, "TTR", 3)) return "Tetraedre Sarl, Epancheurs 34b, 2012 Auvernier, Switzerland";
    if(!strncmp(szMCode, "TTT", 3)) return "Telephone and Telegraph Technique Plc, Sofia, Bulgaria";
    if(!strncmp(szMCode, "TUR", 3)) return "TURKSAY ELEKTRONIK ELEKTRIK ENDUSTRISI";
    if(!strncmp(szMCode, "TXL", 3)) return "CETC46 TianJin New Top Electronics Technology Co.,Ltd., KEYAN East Road 15,Nankai District, Tianjin, China";
    if(!strncmp(szMCode, "UAG", 3)) return "Uher";
    if(!strncmp(szMCode, "UBI", 3)) return "Ubitronix system solutions gmbh, 4232 Hagenberg, Austria";
    if(!strncmp(szMCode, "UEI", 3)) return "United Electrical Industries Limited, Pallimukku, Kollam, India";
    if(!strncmp(szMCode, "UGI", 3)) return "United Gas Industries";
    if(!strncmp(szMCode, "UTI", 3)) return "Utilia Spa, Via Chiabrera, 34/D, Rimini (RN 47924), Italia";
    if(!strncmp(szMCode, "UTL", 3)) return "United Telecoms Limited, A-1/A, 2nd Floor, Revati Building, ECIL XRoads. Hyderabad, INDIA";
    if(!strncmp(szMCode, "VER", 3)) return "VERAUT GmbH, Siemensstr.52, Linz, Austria";
    if(!strncmp(szMCode, "VES", 3)) return "Viterra Energy Services";
    if(!strncmp(szMCode, "VIK", 3)) return "VI-KO ELEKTRIK, Istanbul, Turkey";
    if(!strncmp(szMCode, "VMP", 3)) return "VAMP Oy., Yrittäjänkatu 15 P.O. Box 810 Fl-65101, Vaasa, Finland";
    if(!strncmp(szMCode, "VPI", 3)) return "Van Putten Instruments B.V.";
    if(!strncmp(szMCode, "VSE", 3)) return "Valenciana Smart Energy of Mediterranean Sea S.A, Sir Alexander Fleming, 12 . Warehouse 11, Parque tecnologico de Valencia, 46980, Valencia, Spain";
    if(!strncmp(szMCode, "VTC", 3)) return "Vitelec, Kapittelweg 18, NL 4827 HG Breda, Postbus 6543, NL 4802 HM Breda, Netherlands";
    if(!strncmp(szMCode, "VTK", 3)) return "Linkwell Telesystems Pvt. Ltd., Gowra Kalssic, 1-11-252/1/A, Begumpet, Hyderabad 500016, India";
    if(!strncmp(szMCode, "WAH", 3)) return "WAHESOFT UG, Moeoerte 16, 26316 Varel, Germany";
    if(!strncmp(szMCode, "WAI", 3)) return "Chongqing WECAN Precision Instruments Co.,Ltd, #66 HuangShan Rd,HI-Tech Park , New North Zone, ChongqingP.R.China";
    if(!strncmp(szMCode, "WAL", 3)) return "Wallaby Metering Systems Pvt. Ltd., M-3, 9th Street, Dr.VSI Estate, Thiruvanmiyur, Chennai - 600 041, INDIA";
    if(!strncmp(szMCode, "WEB", 3)) return "Webolution GmbH & Co. KG, Sendenhorsterstrasse 32, 48317 Drensteinfurt, Germany";
    if(!strncmp(szMCode, "WEG", 3)) return "WEG Equipamentos Elétricos S.A. Automação, Av. Pref. Waldemar Grubba, 3000, Jaraguá do Sul, Brazil";
    if(!strncmp(szMCode, "WEH", 3)) return "E. Wehrle GmbH, Obertalstraße 8, 78120 Furtwangen, Germany";
    if(!strncmp(szMCode, "WEL", 3)) return "WELLTECH automation, 263# HongZhong Road, Shanghai,P.R.China";
    if(!strncmp(szMCode, "WFT", 3)) return "Waft Embedded Circuit Solutions, A-109, Sahni Tower, Sector-5, Rajendra Nagar, Sahibabad, Ghaziabad (U.P.), India";
    if(!strncmp(szMCode, "WIN", 3)) return "Windmill Innovations BV, Paasbosweg 14-16, 3862 ZS, Nijkerk (GLD), The Netherlands";
    if(!strncmp(szMCode, "WMO", 3)) return "Westermo Teleindustri AB, Sweden";
    if(!strncmp(szMCode, "WSD", 3)) return "Yantai Wisdom Electric Co., Ltd., JiChang road 2#, Yantai, Shangdong Province, China";
    if(!strncmp(szMCode, "WSE", 3)) return "Changsha Weisheng Electronics Ltd, Changsha, P.R. China";
    if(!strncmp(szMCode, "WTI", 3)) return "Weihai Sunts Electric Meter Co., Ltd, 39-7#, Shenyang Middle Rd. Gaoji Weihai CHINA";
    if(!strncmp(szMCode, "WTL", 3)) return "Wipro Technologies, Doddakannelli, Sarjapur Road. Bangalore, India";
    if(!strncmp(szMCode, "WTM", 3)) return "Watertech S.r.l., Strada dell'Antica Fornace, 2/4, 14053 Canelli (At) Italy";
    if(!strncmp(szMCode, "WZG", 3)) return "Neumann & Co. Wasserzähler Glaubitz GmbH, Industriestraße A7, 01612 Glaubitz, Germany / Deutschland";
    if(!strncmp(szMCode, "WZT", 3)) return "Wizit Co Ltd, Ansin-City Gyeonggi-Do, S Korea";
    if(!strncmp(szMCode, "XAO", 3)) return "Info Solution SpA, Via della Burrona, 51, Vimodrone (MI), Italy";
    if(!strncmp(szMCode, "XEM", 3)) return "XEMTEC AG, Sarnen, Switzerland";
    if(!strncmp(szMCode, "XJM", 3)) return "XJ Metering Co., Ltd, No 416, Ruixiang Road, Xuchang, Henan, China";
    if(!strncmp(szMCode, "XMA", 3)) return "XMASTER s.c. ul. Gersona 41, Wroclaw, Poland";
    if(!strncmp(szMCode, "XMX", 3)) return "Xemex NV, B-2900 Schoten, Belgium";
    if(!strncmp(szMCode, "XTR", 3)) return "HENAN SUNTRONT TECH CO., LTD, No.19 Guohuai Street, High and New Tech Industrial Development Zone, Zhengzhou City, Henan Province, China";
    if(!strncmp(szMCode, "XTY", 3)) return "LianYuanGang Tengyue Electronics & Technology Co.LianYunGang.Jiangsu.china Haizhou, LianYunGang, China";
    if(!strncmp(szMCode, "YDD", 3)) return "Jilin Yongda Group Co., Ltd";
    if(!strncmp(szMCode, "YHE", 3)) return "Youho Electric Co Ltd, Yangjoo, South Korea";
    if(!strncmp(szMCode, "YSS", 3)) return "Yellowstone Soft, Brunnenstr. 32, 89584 Ehingen, Germany";
    if(!strncmp(szMCode, "YTE", 3)) return "Yuksek Teknoloji, Turkey";
    if(!strncmp(szMCode, "YTL", 3)) return "ZheJiang yongtailong electronic co.,ltd, No.8 KangDing Road, Tongxiang, China";
    if(!strncmp(szMCode, "YTL", 3)) return "ZheJiang yongtailong electronic co.,ltd, No.8 KangDing Road, Tongxiang, China";
    if(!strncmp(szMCode, "ZAG", 3)) return "Zellweger Uster AG";
    if(!strncmp(szMCode, "ZAP", 3)) return "Zaptronix";
    if(!strncmp(szMCode, "ZEL", 3)) return "Dr. techn. Josef Zelisko GmbH, Beethovengasse 43 45A-2340 Mödling, Austria";
    if(!strncmp(szMCode, "ZIV", 3)) return "ZIV Aplicaciones y Tecnologia, S.A.";
    if(!strncmp(szMCode, "ZJY", 3)) return "Zhejiang Jiayou Thermal Technology Equipment Co., LTD, Houwan Industrial Point,Yucheng Street,Yuhuan County, Taizhou, China";
    if(!strncmp(szMCode, "ZPA", 3)) return "ZPA Smart Energy a.s., Komenského 821, CZ-541 01 TrutnovCzech Republic";
    if(!strncmp(szMCode, "ZRI", 3)) return "ZENNER International GmbH & Co. KG, Postfach 10 33 39D-66033 Saarbrücken, Germany";
    if(!strncmp(szMCode, "ZRM", 3)) return "ZENNER International GmbH & Co. KG, Postfach 10 33 39D-66033 Saarbrücken, Germany";
    if(!strncmp(szMCode, "ZTY", 3)) return "Hangzhou Chint Meter Technology Co., Ltd, 7th Floor, New Building, No 313, Tianmushan Road, Hangzhou 310013, China";

    return "UNKNOWN";
}

const char * GetManufacturerString(char * szMCode)
{
    if(!szMCode) return "";

    if(strlen(szMCode) < 3) return "UNKNOWN";

    /** Max 32 letters */
    //                                     "12345678901234567890123456789012";
    if(!strncmp(szMCode, "ABB", 3)) return "ABB";
    if(!strncmp(szMCode, "ABN", 3)) return "ABN Braun";
    if(!strncmp(szMCode, "ACA", 3)) return "Acean";
    if(!strncmp(szMCode, "ACE", 3)) return "Actaris";
    if(!strncmp(szMCode, "ACG", 3)) return "Actaris";
    if(!strncmp(szMCode, "ACW", 3)) return "Actaris";
    if(!strncmp(szMCode, "ADD", 3)) return "ADD";
    if(!strncmp(szMCode, "ADN", 3)) return "Aidon";
    if(!strncmp(szMCode, "ADU", 3)) return "Adunos";
    if(!strncmp(szMCode, "ADX", 3)) return "ADD";
    if(!strncmp(szMCode, "AEC", 3)) return "Advance";
    if(!strncmp(szMCode, "AEE", 3)) return "Atlas";
    if(!strncmp(szMCode, "AEG", 3)) return "AEG";
    if(!strncmp(szMCode, "AEL", 3)) return "Kohler";
    if(!strncmp(szMCode, "AEM", 3)) return "S.C. AEM S.A.";
    if(!strncmp(szMCode, "AER", 3)) return "Aerzener";
    if(!strncmp(szMCode, "AFX", 3)) return "Alflex";
    if(!strncmp(szMCode, "AGE", 3)) return "AccessGate";
    if(!strncmp(szMCode, "ALF", 3)) return "Alfatech";
    if(!strncmp(szMCode, "ALG", 3)) return "Algodue";
    if(!strncmp(szMCode, "ALT", 3)) return "Amplitec";
    if(!strncmp(szMCode, "AMB", 3)) return "Amber wireless";
    if(!strncmp(szMCode, "AME", 3)) return "AVON";
    if(!strncmp(szMCode, "AMH", 3)) return "AMiHo";
    if(!strncmp(szMCode, "AMI", 3)) return "AMI Tech";
    if(!strncmp(szMCode, "AML", 3)) return "Eon Electric";
    if(!strncmp(szMCode, "AMP", 3)) return "Ampy Automation Digilog";
    if(!strncmp(szMCode, "AMS", 3)) return "Zhejiang";
    if(!strncmp(szMCode, "AMT", 3)) return "Aquametro";
    if(!strncmp(szMCode, "AMX", 3)) return "APATOR METRIX";
    if(!strncmp(szMCode, "ANA", 3)) return "Anacle";
    if(!strncmp(szMCode, "AND", 3)) return "ANDIS";
    if(!strncmp(szMCode, "APA", 3)) return "APATOR";
    if(!strncmp(szMCode, "APR", 3)) return "Apronecs";
    if(!strncmp(szMCode, "APS", 3)) return "Apsis";
    if(!strncmp(szMCode, "APT", 3)) return "Apator";
    if(!strncmp(szMCode, "APX", 3)) return "Amplex";
    if(!strncmp(szMCode, "AQM", 3)) return "Aquametro";
    if(!strncmp(szMCode, "AQT", 3)) return "AQUATHERM";
    if(!strncmp(szMCode, "ARC", 3)) return "Arcelik";
    if(!strncmp(szMCode, "ARF", 3)) return "ADEUNIS_RF";
    if(!strncmp(szMCode, "ARM", 3)) return "arivus metering";
    if(!strncmp(szMCode, "ARS", 3)) return "ADD - Bulgaria";
    if(!strncmp(szMCode, "ART", 3)) return "Electrotécnica";
    if(!strncmp(szMCode, "ASR", 3)) return "Erelsan";
    if(!strncmp(szMCode, "AST", 3)) return "ASTINCO Inc.";
    if(!strncmp(szMCode, "ATF", 3)) return "AKTIF Otomasyon";
    if(!strncmp(szMCode, "ATI", 3)) return "ANALOGICS TECH";
    if(!strncmp(szMCode, "ATL", 3)) return "Atlas";
    if(!strncmp(szMCode, "ATM", 3)) return "Atmel";
    if(!strncmp(szMCode, "ATS", 3)) return "Atlas";
    if(!strncmp(szMCode, "AUX", 3)) return "Ningbo";
    if(!strncmp(szMCode, "AXI", 3)) return "UAB";
    if(!strncmp(szMCode, "AZE", 3)) return "AZEL";
    if(!strncmp(szMCode, "BAM", 3)) return "Bachmann";
    if(!strncmp(szMCode, "BAR", 3)) return "Baer";
    if(!strncmp(szMCode, "BAS", 3)) return "BASIC INTELLIGENCE";
    if(!strncmp(szMCode, "BBS", 3)) return "BBS";
    if(!strncmp(szMCode, "BCE", 3)) return "ShenZhen";
    if(!strncmp(szMCode, "BEE", 3)) return "Bentec";
    if(!strncmp(szMCode, "BEF", 3)) return "BEFEGA";
    if(!strncmp(szMCode, "BEG", 3)) return "begcomm";
    if(!strncmp(szMCode, "BER", 3)) return "Bernina";
    if(!strncmp(szMCode, "BHG", 3)) return "Brunata";
    if(!strncmp(szMCode, "BKB", 3)) return "Boendekomfort";
    if(!strncmp(szMCode, "BKO", 3)) return "Beko";
    if(!strncmp(szMCode, "BKT", 3)) return "Bekto";
    if(!strncmp(szMCode, "BLU", 3)) return "Bluering";
    if(!strncmp(szMCode, "BME", 3)) return "Beifeng";
    if(!strncmp(szMCode, "BMI", 3)) return "Badger";
    if(!strncmp(szMCode, "BMP", 3)) return "BMETERS";
    if(!strncmp(szMCode, "BMT", 3)) return "BMETERS";
    if(!strncmp(szMCode, "BRA", 3)) return "Brandes";
    if(!strncmp(szMCode, "BSC", 3)) return "Behineh";
    if(!strncmp(szMCode, "BSE", 3)) return "Basari";
    if(!strncmp(szMCode, "BSM", 3)) return "Bluestar";
    if(!strncmp(szMCode, "BSP", 3)) return "Byucksan";
    if(!strncmp(szMCode, "BST", 3)) return "BESTAS";
    if(!strncmp(szMCode, "BSX", 3)) return "BS-Messtechnik";
    if(!strncmp(szMCode, "BTL", 3)) return "BIT-LAB";
    if(!strncmp(szMCode, "BTR", 3)) return "RIA-BTR";
    if(!strncmp(szMCode, "BTS", 3)) return "Basari";
    if(!strncmp(szMCode, "BUR", 3)) return "Bopp";
    if(!strncmp(szMCode, "BXC", 3)) return "Beijing Fuxing Xiao-Cheng";
    if(!strncmp(szMCode, "BYD", 3)) return "BYD";
    if(!strncmp(szMCode, "BYL", 3)) return "BAYLAN";
    if(!strncmp(szMCode, "BZR", 3)) return "Gebr. Bauer";
    if(!strncmp(szMCode, "CAH", 3)) return "CAHORS CEDEX";
    if(!strncmp(szMCode, "CAT", 3)) return "Cubes And Tubes";
    if(!strncmp(szMCode, "CBI", 3)) return "Circuit Breaker";
    if(!strncmp(szMCode, "CDL", 3)) return "Customised";
    if(!strncmp(szMCode, "CEB", 3)) return "Cebyc";
    if(!strncmp(szMCode, "CET", 3)) return "Cetinkaya Aydinlatma";
    if(!strncmp(szMCode, "CGC", 3)) return "Contor Caracioni";
    if(!strncmp(szMCode, "CIR", 3)) return "Circutor";
    if(!strncmp(szMCode, "CLE", 3)) return "Shen Zhen Clou";
    if(!strncmp(szMCode, "CLE", 3)) return "Shen Zhen Clou";
    if(!strncmp(szMCode, "CLO", 3)) return "Clorius";
    if(!strncmp(szMCode, "CLY", 3)) return "Clayster";
    if(!strncmp(szMCode, "CMC", 3)) return "CMC EKOCON";
    if(!strncmp(szMCode, "CMP", 3)) return "CM Partner Inc";
    if(!strncmp(szMCode, "CMV", 3)) return "Comverge";
    if(!strncmp(szMCode, "CNM", 3)) return "COSTEL";
    if(!strncmp(szMCode, "COM", 3)) return "COMMON";
    if(!strncmp(szMCode, "CON", 3)) return "Conlog";
    if(!strncmp(szMCode, "CPL", 3)) return "CPL CONCORDIA";
    if(!strncmp(szMCode, "CPO", 3)) return "C3PO";
    if(!strncmp(szMCode, "CRW", 3)) return "Xuzhou Runwu";
    if(!strncmp(szMCode, "CTR", 3)) return "Contar";
    if(!strncmp(szMCode, "CTX", 3)) return "Contronix";
    if(!strncmp(szMCode, "CUC", 3)) return "cuculus";
    if(!strncmp(szMCode, "CUR", 3)) return "CURRENT Group";
    if(!strncmp(szMCode, "CWI", 3)) return "Cewe Instrument";
    if(!strncmp(szMCode, "CWV", 3)) return "CMEC Electric";
    if(!strncmp(szMCode, "CYE", 3)) return "Quanzhou";
    if(!strncmp(szMCode, "CYI", 3)) return "QUANZHOU";
    if(!strncmp(szMCode, "CYN", 3)) return "Cynox";
    if(!strncmp(szMCode, "CZA", 3)) return "Contazara";
    if(!strncmp(szMCode, "CZM", 3)) return "Cazzaniga";
    if(!strncmp(szMCode, "DAE", 3)) return "DAE Srl";
    if(!strncmp(szMCode, "DAF", 3)) return "Daf";
    if(!strncmp(szMCode, "DAN", 3)) return "Danubia";
    if(!strncmp(szMCode, "DBE", 3)) return "Decibels";
    if(!strncmp(szMCode, "DDE", 3)) return "D&D";
    if(!strncmp(szMCode, "DEC", 3)) return "DECODE";
    if(!strncmp(szMCode, "DEL", 3)) return "DELTAMESS";
    if(!strncmp(szMCode, "DES", 3)) return "Desi";
    if(!strncmp(szMCode, "DEV", 3)) return "Develco";
    if(!strncmp(szMCode, "DFE", 3)) return "Dongfang";
    if(!strncmp(szMCode, "DFS", 3)) return "Danfoss";
    if(!strncmp(szMCode, "DGC", 3)) return "Digicom";
    if(!strncmp(szMCode, "DGM", 3)) return "Diehl";
    if(!strncmp(szMCode, "DIE", 3)) return "Dielen";
    if(!strncmp(szMCode, "DIL", 3)) return "DECCAN";
    if(!strncmp(szMCode, "DJA", 3)) return "De Jaeger Automation";
    if(!strncmp(szMCode, "DKY", 3)) return "Guangdong";
    if(!strncmp(szMCode, "DMC", 3)) return "DMC";
    if(!strncmp(szMCode, "DME", 3)) return "DIEHL Metering";
    if(!strncmp(szMCode, "DMP", 3)) return "DM Power";
    if(!strncmp(szMCode, "DNO", 3)) return "DENO";
    if(!strncmp(szMCode, "DNT", 3)) return "Dr Neuhaus";
    if(!strncmp(szMCode, "DNV", 3)) return "DNV KEMA";
    if(!strncmp(szMCode, "DPP", 3)) return "DECCAN";
    if(!strncmp(szMCode, "DRA", 3)) return "Drachmann";
    if(!strncmp(szMCode, "DRT", 3)) return "DRESSER";
    if(!strncmp(szMCode, "DSE", 3)) return "Digitech";
    if(!strncmp(szMCode, "DSE", 3)) return "DSE";
    if(!strncmp(szMCode, "DWZ", 3)) return "Lorenz";
    if(!strncmp(szMCode, "DZG", 3)) return "Deutsche Zahlergesellschaft";
    if(!strncmp(szMCode, "EAA", 3)) return "Electronic Afzar Azma";
    if(!strncmp(szMCode, "EAH", 3)) return "Endress+Hauser";
    if(!strncmp(szMCode, "EAS", 3)) return "EAS";
    if(!strncmp(szMCode, "ECH", 3)) return "Echelon";
    if(!strncmp(szMCode, "ECL", 3)) return "Electronics";
    if(!strncmp(szMCode, "ECS", 3)) return "Herholdt Controls";
    if(!strncmp(szMCode, "EDI", 3)) return "Enel";
    if(!strncmp(szMCode, "EDM", 3)) return "EDMI Pty.";
    if(!strncmp(szMCode, "EEE", 3)) return "3E";
    if(!strncmp(szMCode, "EEO", 3)) return "Eppeltone";
    if(!strncmp(szMCode, "EFA", 3)) return "EFACEC";
    if(!strncmp(szMCode, "EFE", 3)) return "Engelmann Sensor";
    if(!strncmp(szMCode, "EFR", 3)) return "Europäische Funk-Rundsteuerung";
    if(!strncmp(szMCode, "EGA", 3)) return "eGain";
    if(!strncmp(szMCode, "EGM", 3)) return "Elgama-Elektronika";
    if(!strncmp(szMCode, "EHL", 3)) return "Secure Meters";
    if(!strncmp(szMCode, "EIT", 3)) return "EnergyICT NV";
    if(!strncmp(szMCode, "EKA", 3)) return "Eka Systems";
    if(!strncmp(szMCode, "EKT", 3)) return "PA KVANT J.S.";
    if(!strncmp(szMCode, "ELD", 3)) return "Elektromed";
    if(!strncmp(szMCode, "ELE", 3)) return "Elster";
    if(!strncmp(szMCode, "ELG", 3)) return "Elgas";
    if(!strncmp(szMCode, "ELM", 3)) return "Elektromed";
    if(!strncmp(szMCode, "ELO", 3)) return "ELO Sistemas";
    if(!strncmp(szMCode, "ELQ", 3)) return "ELEQ";
    if(!strncmp(szMCode, "ELR", 3)) return "Elster";
    if(!strncmp(szMCode, "ELS", 3)) return "Elster";
    if(!strncmp(szMCode, "ELT", 3)) return "ELTAKO";
    if(!strncmp(szMCode, "ELV", 3)) return "Elvaco";
    if(!strncmp(szMCode, "EMC", 3)) return "Embedded";
    if(!strncmp(szMCode, "EME", 3)) return "SC. Electromagnetica";
    if(!strncmp(szMCode, "EMH", 3)) return "EMH Elektrizitatszahler";
    if(!strncmp(szMCode, "EML", 3)) return "Emlite";
    if(!strncmp(szMCode, "EMM", 3)) return "Email Metering";
    if(!strncmp(szMCode, "EMO", 3)) return "Enermet";
    if(!strncmp(szMCode, "EMS", 3)) return "EMS-PATVAG";
    if(!strncmp(szMCode, "EMT", 3)) return "Elster Messtechnik";
    if(!strncmp(szMCode, "EMU", 3)) return "EMU Elektronik";
    if(!strncmp(szMCode, "END", 3)) return "ENDYS";
    if(!strncmp(szMCode, "ENE", 3)) return "ENERDIS";
    if(!strncmp(szMCode, "ENG", 3)) return "ENER-G";
    if(!strncmp(szMCode, "ENI", 3)) return "entec innovations";
    if(!strncmp(szMCode, "ENL", 3)) return "ENEL";
    if(!strncmp(szMCode, "ENM", 3)) return "ENMAS";
    if(!strncmp(szMCode, "ENO", 3)) return "ennovatis";
    if(!strncmp(szMCode, "ENP", 3)) return "Kiev";
    if(!strncmp(szMCode, "ENR", 3)) return "Energisme";
    if(!strncmp(szMCode, "ENS", 3)) return "ENSO NETZ";
    if(!strncmp(szMCode, "ENT", 3)) return "ENTES";
    if(!strncmp(szMCode, "ERE", 3)) return "Enermatics Energy";
    if(!strncmp(szMCode, "ERI", 3)) return "Easun Reyrolle Limited";
    if(!strncmp(szMCode, "ERL", 3)) return "Erelsan";
    if(!strncmp(szMCode, "ESE", 3)) return "ESE Nordic";
    if(!strncmp(szMCode, "ESI", 3)) return "Monosan";
    if(!strncmp(szMCode, "ESM", 3)) return "Monosan";
    if(!strncmp(szMCode, "ESO", 3)) return "Monosan";
    if(!strncmp(szMCode, "ESS", 3)) return "Energy Saving Systems";
    if(!strncmp(szMCode, "ESY", 3)) return "EasyMeter";
    if(!strncmp(szMCode, "EUE", 3)) return "E+E";
    if(!strncmp(szMCode, "EUR", 3)) return "Eurometers";
    if(!strncmp(szMCode, "EVK", 3)) return "EV KUR ELEKTRIK";
    if(!strncmp(szMCode, "EWG", 3)) return "EWG DOO";
    if(!strncmp(szMCode, "EWT", 3)) return "Elin Wasserwerkstechnik";
    if(!strncmp(szMCode, "EYT", 3)) return "Enerlyt Potsdam";
    if(!strncmp(szMCode, "FAN", 3)) return "Fantini Cosmi";
    if(!strncmp(szMCode, "FED", 3)) return "Federal Elektrik";
    if(!strncmp(szMCode, "FFD", 3)) return "Fast";
    if(!strncmp(szMCode, "FIM", 3)) return "Frodexim";
    if(!strncmp(szMCode, "FIN", 3)) return "Finder";
    if(!strncmp(szMCode, "FIO", 3)) return "Pietro Fiorentini";
    if(!strncmp(szMCode, "FLE", 3)) return "XI'AN";
    if(!strncmp(szMCode, "FLG", 3)) return "FLOMAG";
    if(!strncmp(szMCode, "FLO", 3)) return "Flonidan";
    if(!strncmp(szMCode, "FLS", 3)) return "FLASH";
    if(!strncmp(szMCode, "FMG", 3)) return "Flow Meter";
    if(!strncmp(szMCode, "FML", 3)) return "Siemens Measurements";
    if(!strncmp(szMCode, "FNX", 3)) return "Flownetix";
    if(!strncmp(szMCode, "FRE", 3)) return "Frer Srl";
    if(!strncmp(szMCode, "FSP", 3)) return "Finmek Space";
    if(!strncmp(szMCode, "FTL", 3)) return "Tritschler";
    if(!strncmp(szMCode, "FUS", 3)) return "Fuccesso";
    if(!strncmp(szMCode, "FUT", 3)) return "first:utility";
    if(!strncmp(szMCode, "FWS", 3)) return "FW Systeme";
    if(!strncmp(szMCode, "GAV", 3)) return "Carlo Gavazzi Controls";
    if(!strncmp(szMCode, "GBJ", 3)) return "Grundfoss";
    if(!strncmp(szMCode, "GCE", 3)) return "Genergica";
    if(!strncmp(szMCode, "GEC", 3)) return "GEC Meters";
    if(!strncmp(szMCode, "GEE", 3)) return "GE Energy";
    if(!strncmp(szMCode, "GEL", 3)) return "ITRI";
    if(!strncmp(szMCode, "GEN", 3)) return "Goerlitz";
    if(!strncmp(szMCode, "GEO", 3)) return "Green Energy";
    if(!strncmp(szMCode, "GET", 3)) return "Genus";
    if(!strncmp(szMCode, "GFM", 3)) return "GE Fuji";
    if(!strncmp(szMCode, "GIL", 3)) return "Genus";
    if(!strncmp(szMCode, "GIN", 3)) return "Gineers";
    if(!strncmp(szMCode, "GMC", 3)) return "Gossen Metrawatt";
    if(!strncmp(szMCode, "GME", 3)) return "Global Metering Electronics";
    if(!strncmp(szMCode, "GMM", 3)) return "Gamma International Egypt";
    if(!strncmp(szMCode, "GMT", 3)) return "GMT";
    if(!strncmp(szMCode, "GOE", 3)) return "Genus Overseas Electronics";
    if(!strncmp(szMCode, "GRE", 3)) return "GE2";
    if(!strncmp(szMCode, "GRI", 3)) return "Grinpal";
    if(!strncmp(szMCode, "GSP", 3)) return "Ingenieurbuero Gasperowicz";
    if(!strncmp(szMCode, "GST", 3)) return "Shenzhen Golden Square";
    if(!strncmp(szMCode, "GTE", 3)) return "GREATech";
    if(!strncmp(szMCode, "GUH", 3)) return "ShenZhen GuangNing";
    if(!strncmp(szMCode, "GWF", 3)) return "Gas-u.";
    if(!strncmp(szMCode, "HAG", 3)) return "Hager Electro";
    if(!strncmp(szMCode, "HCE", 3)) return "Hsiang Cheng Electric Corp";
    if(!strncmp(szMCode, "HEG", 3)) return "Hamburger Elektronik Gesells";
    if(!strncmp(szMCode, "HEI", 3)) return "Hydro-Eco-Invest";
    if(!strncmp(szMCode, "HEL", 3)) return "Heliowatt";
    if(!strncmp(szMCode, "HER", 3)) return "Hermes Systems";
    if(!strncmp(szMCode, "HFR", 3)) return "SAERI HEAT";
    if(!strncmp(szMCode, "HIE", 3)) return "Shenzhen";
    if(!strncmp(szMCode, "HLY", 3)) return "Holley Metering";
    if(!strncmp(szMCode, "HMI", 3)) return "HMI";
    if(!strncmp(szMCode, "HMS", 3)) return "Hermes Systems";
    if(!strncmp(szMCode, "HMU", 3)) return "Hugo";
    if(!strncmp(szMCode, "HND", 3)) return "Shenzhen";
    if(!strncmp(szMCode, "HOE", 3)) return "HOENTZSCH";
    if(!strncmp(szMCode, "HOL", 3)) return "Holosys";
    if(!strncmp(szMCode, "HON", 3)) return "Honeywell";
    if(!strncmp(szMCode, "HPL", 3)) return "HPL-Socomec";
    if(!strncmp(szMCode, "HRM", 3)) return "Hefei Runa";
    if(!strncmp(szMCode, "HRS", 3)) return "HomeRider";
    if(!strncmp(szMCode, "HSD", 3)) return "Ningbo Histar";
    if(!strncmp(szMCode, "HST", 3)) return "HST";
    if(!strncmp(szMCode, "HTC", 3)) return "Horstmann";
    if(!strncmp(szMCode, "HTL", 3)) return "Ernst";
    if(!strncmp(szMCode, "HTS", 3)) return "HTS";
    if(!strncmp(szMCode, "HWT", 3)) return "Huawei";
    if(!strncmp(szMCode, "HXD", 3)) return "Beijing HongHaoXingDa";
    if(!strncmp(szMCode, "HXE", 3)) return "Hexing";
    if(!strncmp(szMCode, "HXW", 3)) return "Hangzhou Xili";
    if(!strncmp(szMCode, "HYD", 3)) return "Hydrometer";
    if(!strncmp(szMCode, "HYE", 3)) return "Zhejiang Hyayi";
    if(!strncmp(szMCode, "HYG", 3)) return "Hydrometer";
    if(!strncmp(szMCode, "HZC", 3)) return "TANGSHAN HUIZHONG";
    if(!strncmp(szMCode, "HZI", 3)) return "TANGSHAN HUIZHONG";
    if(!strncmp(szMCode, "ICM", 3)) return "Intracom";
    if(!strncmp(szMCode, "ICP", 3)) return "PT Indonesia";
    if(!strncmp(szMCode, "ICS", 3)) return "ICSA";
    if(!strncmp(szMCode, "ICT", 3)) return "ICT";
    if(!strncmp(szMCode, "IDE", 3)) return "IMIT";
    if(!strncmp(szMCode, "IEC", 3)) return "leonnardo";
    if(!strncmp(szMCode, "IEE", 3)) return "I.E.";
    if(!strncmp(szMCode, "IFX", 3)) return "Infineon";
    if(!strncmp(szMCode, "IHM", 3)) return "Shenzhen";
    if(!strncmp(szMCode, "IJE", 3)) return "ILJIN";
    if(!strncmp(szMCode, "IMS", 3)) return "IMST";
    if(!strncmp(szMCode, "INC", 3)) return "Incotex";
    if(!strncmp(szMCode, "IND", 3)) return "INDRA";
    if(!strncmp(szMCode, "INE", 3)) return "INNOTAS Elektronik";
    if(!strncmp(szMCode, "INF", 3)) return "Infometric";
    if(!strncmp(szMCode, "INI", 3)) return "Inris";
    if(!strncmp(szMCode, "INP", 3)) return "INNOTAS Produktions";
    if(!strncmp(szMCode, "INS", 3)) return "INSYS";
    if(!strncmp(szMCode, "INT", 3)) return "Infranet";
    if(!strncmp(szMCode, "INV", 3)) return "Sensus";
    if(!strncmp(szMCode, "INX", 3)) return "Innolex";
    if(!strncmp(szMCode, "IPD", 3)) return "IPD";
    if(!strncmp(szMCode, "ISI", 3)) return "Akcionarsko";
    if(!strncmp(szMCode, "ISK", 3)) return "Iskraemeco";
    if(!strncmp(szMCode, "ISO", 3)) return "Isoil";
    if(!strncmp(szMCode, "IST", 3)) return "Ista";
    if(!strncmp(szMCode, "ITA", 3)) return "iTrona";
    if(!strncmp(szMCode, "ITB", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITC", 3)) return "INTECH TUNISIE";
    if(!strncmp(szMCode, "ITE", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITF", 3)) return "ITF";
    if(!strncmp(szMCode, "ITG", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITH", 3)) return "INTELTEH";
    if(!strncmp(szMCode, "ITI", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITR", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITS", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITU", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITW", 3)) return "ITRON";
    if(!strncmp(szMCode, "ITZ", 3)) return "ITRON";
    if(!strncmp(szMCode, "IUS", 3)) return "IUSA";
    if(!strncmp(szMCode, "IWK", 3)) return "IWK";
    if(!strncmp(szMCode, "IZE", 3)) return "iZenze";
    if(!strncmp(szMCode, "JAN", 3)) return "Janitza";
    if(!strncmp(szMCode, "JCE", 3)) return "Janz";
    if(!strncmp(szMCode, "JED", 3)) return "JED";
    if(!strncmp(szMCode, "JGF", 3)) return "Janz";
    if(!strncmp(szMCode, "JHM", 3)) return "Changzhou Jianhu";
    if(!strncmp(szMCode, "JMT", 3)) return "JM-TRONIC";
    if(!strncmp(szMCode, "JOY", 3)) return "Zhejiang";
    if(!strncmp(szMCode, "JUM", 3)) return "JUMO";
    if(!strncmp(szMCode, "KAM", 3)) return "Kamstrup";
    if(!strncmp(szMCode, "KAS", 3)) return "Kamstrup";
    if(!strncmp(szMCode, "KAT", 3)) return "KATHREIN";
    if(!strncmp(szMCode, "KBN", 3)) return "Alpamis";
    if(!strncmp(szMCode, "KEL", 3)) return "KELEMINIC";
    if(!strncmp(szMCode, "KER", 3)) return "KERMS";
    if(!strncmp(szMCode, "KFM", 3)) return "Shenzhen";
    if(!strncmp(szMCode, "KGE", 3)) return "Guangzhou";
    if(!strncmp(szMCode, "KHL", 3)) return "Kohler";
    if(!strncmp(szMCode, "KKE", 3)) return "KK";
    if(!strncmp(szMCode, "KKE", 3)) return "KK-Electronic";
    if(!strncmp(szMCode, "KMB", 3)) return "Kamstrup";
    if(!strncmp(szMCode, "KMT", 3)) return "Krohne Messtechnik";
    if(!strncmp(szMCode, "KNX", 3)) return "KONNEX";
    if(!strncmp(szMCode, "KRO", 3)) return "Kromschroder";
    if(!strncmp(szMCode, "KST", 3)) return "KUNDO";
    if(!strncmp(szMCode, "KSY", 3)) return "KUNDO";
    if(!strncmp(szMCode, "KTC", 3)) return "Kerman";
    if(!strncmp(szMCode, "LAC", 3)) return "Heinz";
    if(!strncmp(szMCode, "LAN", 3)) return "Langmatz";
    if(!strncmp(szMCode, "LCG", 3)) return "Landis+Gyr";
    if(!strncmp(szMCode, "LCR", 3)) return "ShanDong LiChuang";
    if(!strncmp(szMCode, "LDE", 3)) return "Shenzhen";
    if(!strncmp(szMCode, "LEC", 3)) return "Lectrotek";
    if(!strncmp(szMCode, "LEM", 3)) return "LEM HEME";
    if(!strncmp(szMCode, "LGB", 3)) return "Landis+Gyr";
    if(!strncmp(szMCode, "LGD", 3)) return "Landis+Gyr";
    if(!strncmp(szMCode, "LGS", 3)) return "Landis+Gyr";
    if(!strncmp(szMCode, "LGU", 3)) return "LG Uplus";
    if(!strncmp(szMCode, "LGZ", 3)) return "Landis+Gyr";
    if(!strncmp(szMCode, "LHA", 3)) return "Atlantic";
    if(!strncmp(szMCode, "LML", 3)) return "LUMEL";
    if(!strncmp(szMCode, "LNT", 3)) return "Larsen";
    if(!strncmp(szMCode, "LSE", 3)) return "Landis";
    if(!strncmp(szMCode, "LSK", 3)) return "LS";
    if(!strncmp(szMCode, "LSP", 3)) return "Landis+Gyr";
    if(!strncmp(szMCode, "LSZ", 3)) return "Siemens";
    if(!strncmp(szMCode, "LUG", 3)) return "Landis+Gyr";
    if(!strncmp(szMCode, "LUN", 3)) return "Protokol";
    if(!strncmp(szMCode, "LYE", 3)) return "Jiangsu";
    if(!strncmp(szMCode, "MAC", 3)) return "RUDNAP";
    if(!strncmp(szMCode, "MAD", 3)) return "Maddalena";
    if(!strncmp(szMCode, "MAE", 3)) return "Mates";
    if(!strncmp(szMCode, "MAN", 3)) return "Manthey";
    if(!strncmp(szMCode, "MAT", 3)) return "Mitsubishi";
    if(!strncmp(szMCode, "MAX", 3)) return "MAXMET";
    if(!strncmp(szMCode, "MBS", 3)) return "MBS";
    if(!strncmp(szMCode, "MCR", 3)) return "MICRORISC";
    if(!strncmp(szMCode, "MDE", 3)) return "Diehl";
    if(!strncmp(szMCode, "MEC", 3)) return "Mitsubishi";
    if(!strncmp(szMCode, "MED", 3)) return "MAHARASHTRA";
    if(!strncmp(szMCode, "MEH", 3)) return "Mueller";
    if(!strncmp(szMCode, "MEI", 3)) return "Sensus";
    if(!strncmp(szMCode, "MEL", 3)) return "Mikroelektronika";
    if(!strncmp(szMCode, "MEM", 3)) return "MEMS";
    if(!strncmp(szMCode, "MET", 3)) return "METRA";
    if(!strncmp(szMCode, "MIC", 3)) return "Microtronics";
    if(!strncmp(szMCode, "MII", 3)) return "Miitors";
    if(!strncmp(szMCode, "MIM", 3)) return "Malaysian";
    if(!strncmp(szMCode, "MIR", 3)) return "MIR";
    if(!strncmp(szMCode, "MIS", 3)) return "Iskra";
    if(!strncmp(szMCode, "MKE", 3)) return "MKEK";
    if(!strncmp(szMCode, "MKL", 3)) return "MAKEL";
    if(!strncmp(szMCode, "MKS", 3)) return "MAK-SAY";
    if(!strncmp(szMCode, "MMC", 3)) return "Modern";
    if(!strncmp(szMCode, "MMI", 3)) return "MyMeterInfo";
    if(!strncmp(szMCode, "MMS", 3)) return "Brunswick";
    if(!strncmp(szMCode, "MNS", 3)) return "MANAS";
    if(!strncmp(szMCode, "MOS", 3)) return "MOMAS";
    if(!strncmp(szMCode, "MPA", 3)) return "Mega Power";
    if(!strncmp(szMCode, "MPR", 3)) return "Michael";
    if(!strncmp(szMCode, "MPS", 3)) return "Multiprocessor";
    if(!strncmp(szMCode, "MRT", 3)) return "MIRTEK";
    if(!strncmp(szMCode, "MSB", 3)) return "MISA SDN BHD";
    if(!strncmp(szMCode, "MSE", 3)) return "Mahashakti";
    if(!strncmp(szMCode, "MSM", 3)) return "MS-M";
    if(!strncmp(szMCode, "MSO", 3)) return "Metiso";
    if(!strncmp(szMCode, "MSY", 3)) return "MAK-SAY";
    if(!strncmp(szMCode, "MTC", 3)) return "Metering";
    if(!strncmp(szMCode, "MTH", 3)) return "njmeter";
    if(!strncmp(szMCode, "MTI", 3)) return "Micrtotech";
    if(!strncmp(szMCode, "MTM", 3)) return "Metrum";
    if(!strncmp(szMCode, "MTS", 3)) return "MeteRSit";
    if(!strncmp(szMCode, "MTX", 3)) return "Matrix";
    if(!strncmp(szMCode, "MUK", 3)) return "Meters";
    if(!strncmp(szMCode, "MWU", 3)) return "METRONA";
    if(!strncmp(szMCode, "MXM", 3)) return "Maxim";
    if(!strncmp(szMCode, "NAR", 3)) return "NARI";
    if(!strncmp(szMCode, "NDF", 3)) return "NÚCLEO DURO FELGUERA";
    if(!strncmp(szMCode, "NDM", 3)) return "Northern Design";
    if(!strncmp(szMCode, "NES", 3)) return "NORA";
    if(!strncmp(szMCode, "NIS", 3)) return "Nisko";
    if(!strncmp(szMCode, "NJC", 3)) return "NAMJUN";
    if(!strncmp(szMCode, "NMS", 3)) return "Nisko";
    if(!strncmp(szMCode, "NNT", 3)) return "2N Telekomunikace";
    if(!strncmp(szMCode, "NRM", 3)) return "Norm";
    if(!strncmp(szMCode, "NTC", 3)) return "Nuri";
    if(!strncmp(szMCode, "NTM", 3)) return "Netinium";
    if(!strncmp(szMCode, "NVN", 3)) return "NOVEN";
    if(!strncmp(szMCode, "NWM", 3)) return "Ningbo Water Meter";
    if(!strncmp(szMCode, "NXP", 3)) return "NXP";
    if(!strncmp(szMCode, "NYG", 3)) return "Ningbo Yonggang";
    if(!strncmp(szMCode, "NYN", 3)) return "Nanjing Yuneng";
    if(!strncmp(szMCode, "NZR", 3)) return "Nordwestdeutsche";
    if(!strncmp(szMCode, "OAS", 3)) return "Omni";
    if(!strncmp(szMCode, "ODI", 3)) return "OAS Digital";
    if(!strncmp(szMCode, "OEE", 3)) return "ONUR";
    if(!strncmp(szMCode, "OMS", 3)) return "OMNISYSTEM";
    if(!strncmp(szMCode, "ONR", 3)) return "ONUR";
    if(!strncmp(szMCode, "ONS", 3)) return "ONUR";
    if(!strncmp(szMCode, "OPT", 3)) return "Optec";
    if(!strncmp(szMCode, "ORB", 3)) return "ORBIS";
    if(!strncmp(szMCode, "ORM", 3)) return "Ormazabal";
    if(!strncmp(szMCode, "OSA", 3)) return "Osaki";
    if(!strncmp(szMCode, "OSK", 3)) return "Osaki";
    if(!strncmp(szMCode, "OZK", 3)) return "Oz-kar";
    if(!strncmp(szMCode, "PAD", 3)) return "PadMess";
    if(!strncmp(szMCode, "PAF", 3)) return "FAP";
    if(!strncmp(szMCode, "PAK", 3)) return "Paktim";
    if(!strncmp(szMCode, "PAN", 3)) return "Panasonic";
    if(!strncmp(szMCode, "PDX", 3)) return "Paradox";
    if(!strncmp(szMCode, "PEL", 3)) return "Pak";
    if(!strncmp(szMCode, "PGP", 3)) return "P.G.P.";
    if(!strncmp(szMCode, "PHL", 3)) return "Hangzhou";
    if(!strncmp(szMCode, "PII", 3)) return "PiiGAB";
    if(!strncmp(szMCode, "PIK", 3)) return "pikkerton";
    if(!strncmp(szMCode, "PIL", 3)) return "Pilot";
    if(!strncmp(szMCode, "PIM", 3)) return "Power Innovation";
    if(!strncmp(szMCode, "PIP", 3)) return "Hermann";
    if(!strncmp(szMCode, "PLO", 3)) return "Weihai";
    if(!strncmp(szMCode, "PMG", 3)) return "Sensus";
    if(!strncmp(szMCode, "POD", 3)) return "PowerOneData";
    if(!strncmp(szMCode, "POW", 3)) return "PowerApp";
    if(!strncmp(szMCode, "POZ", 3)) return "ZEUP Pozyton";
    if(!strncmp(szMCode, "PPC", 3)) return "Power Plus";
    if(!strncmp(szMCode, "PPS", 3)) return "Palace Power";
    if(!strncmp(szMCode, "PRE", 3)) return "Predicate";
    if(!strncmp(szMCode, "PRG", 3)) return "Paud Raad";
    if(!strncmp(szMCode, "PRI", 3)) return "Polymeters";
    if(!strncmp(szMCode, "PRO", 3)) return "Proton";
    if(!strncmp(szMCode, "PST", 3)) return "PSTec";
    if(!strncmp(szMCode, "PWB", 3)) return "Paul Wegener";
    if(!strncmp(szMCode, "PWR", 3)) return "Powrtec";
    if(!strncmp(szMCode, "PYU", 3)) return "PYUNGIL";
    if(!strncmp(szMCode, "QDS", 3)) return "Qundis";
    if(!strncmp(szMCode, "QTS", 3)) return "QT";
    if(!strncmp(szMCode, "RAC", 3)) return "Michael";
    if(!strncmp(szMCode, "RAD", 3)) return "Radiocrafts";
    if(!strncmp(szMCode, "RAM", 3)) return "Rossweiner";
    if(!strncmp(szMCode, "RAS", 3)) return "Rubikon Apskaitos";
    if(!strncmp(szMCode, "RCE", 3)) return "RC";
    if(!strncmp(szMCode, "REC", 3)) return "Zhejiang";
    if(!strncmp(szMCode, "REL", 3)) return "Relay";
    if(!strncmp(szMCode, "REM", 3)) return "REM";
    if(!strncmp(szMCode, "RIC", 3)) return "Richa Equipments";
    if(!strncmp(szMCode, "RIL", 3)) return "Rikken";
    if(!strncmp(szMCode, "RIM", 3)) return "CJSC Radio and Microelectronics";
    if(!strncmp(szMCode, "RIT", 3)) return "Ritz";
    if(!strncmp(szMCode, "RIZ", 3)) return "RIZ";
    if(!strncmp(szMCode, "RKE", 3)) return "Viterra";
    if(!strncmp(szMCode, "RML", 3)) return "ROLEX";
    if(!strncmp(szMCode, "RMR", 3)) return "RAMAR";
    if(!strncmp(szMCode, "RSA", 3)) return "Rahrovan";
    if(!strncmp(szMCode, "RSW", 3)) return "RSW";
    if(!strncmp(szMCode, "SAA", 3)) return "Sanjesh";
    if(!strncmp(szMCode, "SAC", 3)) return "Sacofgas";
    if(!strncmp(szMCode, "SAG", 3)) return "SAGEM";
    if(!strncmp(szMCode, "SAM", 3)) return "Siemens";
    if(!strncmp(szMCode, "SAP", 3)) return "Sappel ";
    if(!strncmp(szMCode, "SAT", 3)) return "SATEC";
    if(!strncmp(szMCode, "SBC", 3)) return "Saia-Burgess";
    if(!strncmp(szMCode, "SCE", 3)) return "Seo Chang";
    if(!strncmp(szMCode, "SCH", 3)) return "Schinzel";
    if(!strncmp(szMCode, "SCW", 3)) return "ScatterWeb";
    if(!strncmp(szMCode, "SDC", 3)) return "SdC";
    if(!strncmp(szMCode, "SDM", 3)) return "Shandong Delu";
    if(!strncmp(szMCode, "SEC", 3)) return "Schneider";
    if(!strncmp(szMCode, "SEE", 3)) return "Sewedy Electrometer";
    if(!strncmp(szMCode, "SEN", 3)) return "Sensus";
    if(!strncmp(szMCode, "SGA", 3)) return "smartGAS";
    if(!strncmp(szMCode, "SGM", 3)) return "Swiss";
    if(!strncmp(szMCode, "SHD", 3)) return "Beijing SanHuaDeBao";
    if(!strncmp(szMCode, "SHE", 3)) return "Shenzhen";
    if(!strncmp(szMCode, "SHM", 3)) return "Shanghai";
    if(!strncmp(szMCode, "SIE", 3)) return "Siemens";
    if(!strncmp(szMCode, "SIT", 3)) return "SITEL";
    if(!strncmp(szMCode, "SLB", 3)) return "Schlumberger";
    if(!strncmp(szMCode, "SLP", 3)) return "Sylop";
    if(!strncmp(szMCode, "SLX", 3)) return "Ymatron";
    if(!strncmp(szMCode, "SMC", 3)) return "Pending";
    if(!strncmp(szMCode, "SME", 3)) return "Siame";
    if(!strncmp(szMCode, "SMG", 3)) return "Samgas";
    if(!strncmp(szMCode, "SML", 3)) return "Siemens";
    if(!strncmp(szMCode, "SMT", 3)) return "Smarteh";
    if(!strncmp(szMCode, "SNM", 3)) return "ShenZhen";
    if(!strncmp(szMCode, "SNR", 3)) return "NTN-SNR";
    if(!strncmp(szMCode, "SNS", 3)) return "Signals";
    if(!strncmp(szMCode, "SOC", 3)) return "SOCOMEC";
    if(!strncmp(szMCode, "SOF", 3)) return "Softflow.de";
    if(!strncmp(szMCode, "SOG", 3)) return "Sogecam";
    if(!strncmp(szMCode, "SOL", 3)) return "Soledia";
    if(!strncmp(szMCode, "SOM", 3)) return "Somesca";
    if(!strncmp(szMCode, "SON", 3)) return "Sontex";
    if(!strncmp(szMCode, "SPL", 3)) return "Sappel";
    if(!strncmp(szMCode, "SPX", 3)) return "Sensus";
    if(!strncmp(szMCode, "SRE", 3)) return "Guangzhou";
    if(!strncmp(szMCode, "SRF", 3)) return "Saraf";
    if(!strncmp(szMCode, "SRN", 3)) return "Shandong SARON";
    if(!strncmp(szMCode, "SRV", 3)) return "Servic";
    if(!strncmp(szMCode, "SSN", 3)) return "Silver";
    if(!strncmp(szMCode, "SST", 3)) return "Qingdao Haina";
    if(!strncmp(szMCode, "STA", 3)) return "Shenzhen Star";
    if(!strncmp(szMCode, "STC", 3)) return "Sunrise";
    if(!strncmp(szMCode, "STD", 3)) return "Stedin";
    if(!strncmp(szMCode, "STR", 3)) return "Strike";
    if(!strncmp(szMCode, "STV", 3)) return "STV";
    if(!strncmp(szMCode, "STZ", 3)) return "Steinbeis";
    if(!strncmp(szMCode, "SVM", 3)) return "SVM";
    if(!strncmp(szMCode, "SWI", 3)) return "Swistec";
    if(!strncmp(szMCode, "SWT", 3)) return "Beijing Swirling";
    if(!strncmp(szMCode, "SYN", 3)) return "SMSISTEM";
    if(!strncmp(szMCode, "TAG", 3)) return "Telma";
    if(!strncmp(szMCode, "TAT", 3)) return "Tatung";
    if(!strncmp(szMCode, "TAY", 3)) return "Taytech Otomasyon";
    if(!strncmp(szMCode, "TCE", 3)) return "Qindao";
    if(!strncmp(szMCode, "TCH", 3)) return "Techem Service";
    if(!strncmp(szMCode, "TDC", 3)) return "Telecom Design";
    if(!strncmp(szMCode, "TEC", 3)) return "TECSON";
    if(!strncmp(szMCode, "TEP", 3)) return "TEPEECAL";
    if(!strncmp(szMCode, "TFC", 3)) return "Toos Fuse";
    if(!strncmp(szMCode, "THE", 3)) return "Theben";
    if(!strncmp(szMCode, "TIP", 3)) return "TIP";
    if(!strncmp(szMCode, "TIX", 3)) return "Tixi.Com";
    if(!strncmp(szMCode, "TLM", 3)) return "Theodor";
    if(!strncmp(szMCode, "TMK", 3)) return "Timi Kosova";
    if(!strncmp(szMCode, "TMS", 3)) return "TEMASS IMALAT";
    if(!strncmp(szMCode, "TPL", 3)) return "Teplocom";
    if(!strncmp(szMCode, "TRI", 3)) return "Tritech";
    if(!strncmp(szMCode, "TRJ", 3)) return "SHENZHEN TECHRISE";
    if(!strncmp(szMCode, "TRL", 3)) return "Trilliant";
    if(!strncmp(szMCode, "TRV", 3)) return "Transvaro";
    if(!strncmp(szMCode, "TSD", 3)) return "Theobroma";
    if(!strncmp(szMCode, "TTM", 3)) return "Toshiba Toko Meter";
    if(!strncmp(szMCode, "TTR", 3)) return "Tetraedre";
    if(!strncmp(szMCode, "TTT", 3)) return "Telephone";
    if(!strncmp(szMCode, "TUR", 3)) return "TURKSAY";
    if(!strncmp(szMCode, "TXL", 3)) return "CETC46 TianJin";
    if(!strncmp(szMCode, "UAG", 3)) return "Uher";
    if(!strncmp(szMCode, "UBI", 3)) return "Ubitronix";
    if(!strncmp(szMCode, "UEI", 3)) return "United Electrical";
    if(!strncmp(szMCode, "UGI", 3)) return "United Gas Industries";
    if(!strncmp(szMCode, "UTI", 3)) return "Utilia";
    if(!strncmp(szMCode, "UTL", 3)) return "United Telecoms";
    if(!strncmp(szMCode, "VER", 3)) return "VERAUT";
    if(!strncmp(szMCode, "VES", 3)) return "Viterra Energy Services";
    if(!strncmp(szMCode, "VIK", 3)) return "VI-KO";
    if(!strncmp(szMCode, "VMP", 3)) return "VAMP";
    if(!strncmp(szMCode, "VPI", 3)) return "Van Putten Instruments";
    if(!strncmp(szMCode, "VSE", 3)) return "Valenciana Smart Energy";
    if(!strncmp(szMCode, "VTC", 3)) return "Vitelec";
    if(!strncmp(szMCode, "VTK", 3)) return "Linkwell";
    if(!strncmp(szMCode, "WAH", 3)) return "WAHESOFT";
    if(!strncmp(szMCode, "WAI", 3)) return "Chongqing WECAN";
    if(!strncmp(szMCode, "WAL", 3)) return "Wallaby";
    if(!strncmp(szMCode, "WEB", 3)) return "Webolution";
    if(!strncmp(szMCode, "WEG", 3)) return "WEG";
    if(!strncmp(szMCode, "WEH", 3)) return "E. Wehrle";
    if(!strncmp(szMCode, "WEL", 3)) return "WELLTECH automation";
    if(!strncmp(szMCode, "WFT", 3)) return "Waft Embedded Circuit Solutions";
    if(!strncmp(szMCode, "WIN", 3)) return "Windmill";
    if(!strncmp(szMCode, "WMO", 3)) return "Westermo";
    if(!strncmp(szMCode, "WSD", 3)) return "Yantai";
    if(!strncmp(szMCode, "WSE", 3)) return "Changsha Weisheng";
    if(!strncmp(szMCode, "WTI", 3)) return "Weihai";
    if(!strncmp(szMCode, "WTL", 3)) return "Wipro";
    if(!strncmp(szMCode, "WTM", 3)) return "Watertech";
    if(!strncmp(szMCode, "WZG", 3)) return "Neumann";
    if(!strncmp(szMCode, "WZT", 3)) return "Wizit";
    if(!strncmp(szMCode, "XAO", 3)) return "Info Solution";
    if(!strncmp(szMCode, "XEM", 3)) return "XEMTEC";
    if(!strncmp(szMCode, "XJM", 3)) return "XJ Metering";
    if(!strncmp(szMCode, "XMA", 3)) return "XMASTER";
    if(!strncmp(szMCode, "XMX", 3)) return "Xemex";
    if(!strncmp(szMCode, "XTR", 3)) return "HENAN SUNTRONT";
    if(!strncmp(szMCode, "XTY", 3)) return "LianYuanGang";
    if(!strncmp(szMCode, "YDD", 3)) return "Jilin Yongda";
    if(!strncmp(szMCode, "YHE", 3)) return "Youho";
    if(!strncmp(szMCode, "YSS", 3)) return "Yellowstone";
    if(!strncmp(szMCode, "YTE", 3)) return "Yuksek Teknoloji";
    if(!strncmp(szMCode, "YTL", 3)) return "ZheJiang yongtailong";
    if(!strncmp(szMCode, "YTL", 3)) return "ZheJiang";
    if(!strncmp(szMCode, "ZAG", 3)) return "Zellweger Uster";
    if(!strncmp(szMCode, "ZAP", 3)) return "Zaptronix";
    if(!strncmp(szMCode, "ZEL", 3)) return "Dr. techn";
    if(!strncmp(szMCode, "ZIV", 3)) return "ZIV";
    if(!strncmp(szMCode, "ZJY", 3)) return "Zhejiang Jiayou";
    if(!strncmp(szMCode, "ZPA", 3)) return "ZPA";
    if(!strncmp(szMCode, "ZRI", 3)) return "ZENNER";
    if(!strncmp(szMCode, "ZRM", 3)) return "ZENNER";
    if(!strncmp(szMCode, "ZTY", 3)) return "Hangzhou";

    return "UNKNOWN";
}

BYTE GetManufacturerVendorCode(char * szMCode)
{
    if(!szMCode) return METER_VENDOR_UNKNOWN;

    if(strlen(szMCode) < 3) return METER_VENDOR_UNKNOWN;

    /** Max 32 letters */
    //                                  "12345678901234567890123456789012";
    /////////////////////////////////////////////////////////////////////////
    if(!strncmp(szMCode, "ABB", 3)) return METER_VENDOR_ABB;
    if(!strncmp(szMCode, "ABN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ACA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ACE", 3)) return METER_VENDOR_ACTARIS;
    if(!strncmp(szMCode, "ACG", 3)) return METER_VENDOR_ACTARIS;
    if(!strncmp(szMCode, "ACW", 3)) return METER_VENDOR_ACTARIS;
    if(!strncmp(szMCode, "ADD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ADN", 3)) return METER_VENDOR_AIDON;
    if(!strncmp(szMCode, "ADU", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ADX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AEC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AEE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AEG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AEM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AER", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AFX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AGE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ALF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ALG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ALT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AMB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AME", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AMH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AMI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AML", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AMP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AMS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AMT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AMX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ANA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AND", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "APA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "APR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "APS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "APT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "APX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AQM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AQT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ARC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ARF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ARM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ARS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ART", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ASR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AST", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ATF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ATI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ATL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ATM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ATS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AUX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AXI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "AZE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BAM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BAR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BAS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BBS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BCE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BEE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BEF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BEG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BER", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BHG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BKB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BKO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BKT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BLU", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BME", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BMI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BMP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BMT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BRA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BSC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BSE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BSM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BSP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BST", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BSX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BTL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BTR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BTS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BUR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BXC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BYD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BYL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "BZR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CAH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CAT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CBI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CDL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CEB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CET", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CGC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CIR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CLE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CLE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CLO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CLY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CMC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CMP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CMV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CNM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "COM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CON", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CPL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CPO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CRW", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CTR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CTX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CUC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CUR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CWI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CWV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CYE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CYI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CYN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CZA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "CZM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DAE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DAF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DAN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DBE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DDE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DEC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DES", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DEV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DFE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DFS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DGC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DGM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DIE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DIL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DJA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DKY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DMC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DME", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DMP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DNO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DNT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DNV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DPP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DRA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DRT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DSE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DSE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DWZ", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "DZG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EAA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EAH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EAS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ECH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ECL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ECS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EDI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EDM", 3)) return METER_VENDOR_EDMI;
    if(!strncmp(szMCode, "EEE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EEO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EFA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EFE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EFR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EGA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EGM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EHL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EIT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EKA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EKT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELE", 3)) return METER_VENDOR_ELSTER;
    if(!strncmp(szMCode, "ELG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELQ", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELR", 3)) return METER_VENDOR_ELSTER;
    if(!strncmp(szMCode, "ELS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ELV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EMC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EME", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EMH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EML", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EMM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EMO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EMS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EMT", 3)) return METER_VENDOR_ELSTER;
    if(!strncmp(szMCode, "EMU", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "END", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ENT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ERE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ERI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ERL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ESE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ESI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ESM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ESO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ESS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ESY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EUE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EUR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EVK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EWG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EWT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "EYT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FAN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FED", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FFD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FIM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FIN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FIO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FLE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FLG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FLO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FLS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FMG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FML", 3)) return METER_VENDOR_SIEMENS;
    if(!strncmp(szMCode, "FNX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FRE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FSP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FTL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FUS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FUT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "FWS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GAV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GBJ", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GCE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GEC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GEE", 3)) return METER_VENDOR_GE;
    if(!strncmp(szMCode, "GEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GEN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GEO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GET", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GFM", 3)) return METER_VENDOR_GE;
    if(!strncmp(szMCode, "GIL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GIN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GMC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GME", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GMM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GMT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GOE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GRE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GRI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GSP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GST", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GTE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GUH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "GWF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HAG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HCE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HEG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HEI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HER", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HFR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HIE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HLY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HMI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HMS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HMU", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HND", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HOE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HOL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HON", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HPL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HRM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HRS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HSD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HST", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HTC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HTL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HTS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HWT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HXD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HXE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HXW", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HYD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HYE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HYG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HZC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "HZI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ICM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ICP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ICS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ICT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IDE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IEC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IEE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IFX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IHM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IJE", 3)) return METER_VENDOR_ILJINE;
    if(!strncmp(szMCode, "IMS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IND", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "INV", 3)) return METER_VENDOR_SENSUS;
    if(!strncmp(szMCode, "INX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IPD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ISI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ISK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ISO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IST", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ITA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ITB", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ITE", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ITG", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ITI", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITR", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITS", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITU", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITW", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "ITZ", 3)) return METER_VENDOR_ITRON;
    if(!strncmp(szMCode, "IUS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IWK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "IZE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JAN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JCE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JED", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JGF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JHM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JMT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JOY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "JUM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KAM", 3)) return METER_VENDOR_KAMSTRUP;
    if(!strncmp(szMCode, "KAS", 3)) return METER_VENDOR_KAMSTRUP;
    if(!strncmp(szMCode, "KAT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KBN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KER", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KFM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KGE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KHL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KKE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KKE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KMB", 3)) return METER_VENDOR_KAMSTRUP;
    if(!strncmp(szMCode, "KMT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KNX", 3)) return METER_VENDOR_SIEMENS;
    if(!strncmp(szMCode, "KRO", 3)) return METER_VENDOR_KROMSCHRODER;
    if(!strncmp(szMCode, "KST", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KSY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "KTC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LAC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LAN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LCG", 3)) return METER_VENDOR_LANDIS_GYR;
    if(!strncmp(szMCode, "LCR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LDE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LEC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LEM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LGB", 3)) return METER_VENDOR_LANDIS_GYR;
    if(!strncmp(szMCode, "LGD", 3)) return METER_VENDOR_LANDIS_GYR;
    if(!strncmp(szMCode, "LGS", 3)) return METER_VENDOR_LANDIS_GYR;
    if(!strncmp(szMCode, "LGU", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LGZ", 3)) return METER_VENDOR_LANDIS_GYR;
    if(!strncmp(szMCode, "LHA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LML", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LNT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LSE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LSK", 3)) return METER_VENDOR_LSIS;
    if(!strncmp(szMCode, "LSP", 3)) return METER_VENDOR_LANDIS_GYR;
    if(!strncmp(szMCode, "LSZ", 3)) return METER_VENDOR_SIEMENS;
    if(!strncmp(szMCode, "LUG", 3)) return METER_VENDOR_LANDIS_GYR;
    if(!strncmp(szMCode, "LUN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "LYE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MAC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MAD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MAE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MAN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MAT", 3)) return METER_VENDOR_MITSUBISHI;
    if(!strncmp(szMCode, "MAX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MBS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MCR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MDE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MEC", 3)) return METER_VENDOR_MITSUBISHI;
    if(!strncmp(szMCode, "MED", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MEH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MEI", 3)) return METER_VENDOR_SENSUS;
    if(!strncmp(szMCode, "MEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MEM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MET", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MIC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MII", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MIM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MIR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MIS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MKE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MKL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MKS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MMC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MMI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MMS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MNS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MOS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MPA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MPR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MPS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MRT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MSB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MSE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MSM", 3)) return METER_VENDOR_MSM;
    if(!strncmp(szMCode, "MSO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MSY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MTC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MTH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MTI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MTM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MTS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MTX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MUK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MWU", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "MXM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NAR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NDF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NDM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NES", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NIS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NJC", 3)) return METER_VENDOR_NAMJUN;
    if(!strncmp(szMCode, "NMS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NNT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NRM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NTC", 3)) return METER_VENDOR_NURI;
    if(!strncmp(szMCode, "NTM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NVN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NWM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NXP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NYG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NYN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "NZR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "OAS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ODI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "OEE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "OMS", 3)) return METER_VENDOR_OMNI;
    if(!strncmp(szMCode, "ONR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ONS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "OPT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ORB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ORM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "OSA", 3)) return METER_VENDOR_OSAKI;
    if(!strncmp(szMCode, "OSK", 3)) return METER_VENDOR_OSAKI;
    if(!strncmp(szMCode, "OZK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PAD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PAF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PAK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PAN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PDX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PGP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PHL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PII", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PIK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PIL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PIM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PIP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PLO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PMG", 3)) return METER_VENDOR_SENSUS;
    if(!strncmp(szMCode, "POD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "POW", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "POZ", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PPC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PPS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PRE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PRG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PRI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PRO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PST", 3)) return METER_VENDOR_PSTEC;
    if(!strncmp(szMCode, "PWB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PWR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "PYU", 3)) return METER_VENDOR_PYUNGIL;
    if(!strncmp(szMCode, "QDS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "QTS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RAC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RAD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RAM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RAS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RCE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "REC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "REL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "REM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RIC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RIL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RIM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RIT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RIZ", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RKE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RML", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RMR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RSA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "RSW", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SAA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SAC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SAG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SAM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SAP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SAT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SBC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SCE", 3)) return METER_VENDOR_SEOCHANG;
    if(!strncmp(szMCode, "SCH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SCW", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SDC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SDM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SEC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SEE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SEN", 3)) return METER_VENDOR_SENSUS;
    if(!strncmp(szMCode, "SGA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SGM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SHD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SHE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SHM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SIE", 3)) return METER_VENDOR_SIEMENS;
    if(!strncmp(szMCode, "SIT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SLB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SLP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SLX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SMC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SME", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SMG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SML", 3)) return METER_VENDOR_SIEMENS;
    if(!strncmp(szMCode, "SMT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SNM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SNR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SNS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SOC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SOF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SOG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SOL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SOM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SON", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SPL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SPX", 3)) return METER_VENDOR_SENSUS;
    if(!strncmp(szMCode, "SRE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SRF", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SRN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SRV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SSN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SST", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "STA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "STC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "STD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "STR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "STV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "STZ", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SVM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SWI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SWT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "SYN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TAG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TAT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TAY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TCE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TCH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TDC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TEC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TEP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TFC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "THE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TIP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TIX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TLM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TMK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TMS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TPL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TRI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TRJ", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TRL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TRV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TSD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TTM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TTR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TTT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TUR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "TXL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "UAG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "UBI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "UEI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "UGI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "UTI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "UTL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VER", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VES", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VIK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VMP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VPI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VSE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VTC", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "VTK", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WAH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WAI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WAL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WEB", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WEG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WEH", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WFT", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WIN", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WMO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WSD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WSE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WTI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WTL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WTM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WZG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "WZT", 3)) return METER_VENDOR_WIZIT;
    if(!strncmp(szMCode, "XAO", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "XEM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "XJM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "XMA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "XMX", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "XTR", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "XTY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "YDD", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "YHE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "YSS", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "YTE", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "YTL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "YTL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZAG", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZAP", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZEL", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZIV", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZJY", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZPA", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZRI", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZRM", 3)) return METER_VENDOR_ETC;
    if(!strncmp(szMCode, "ZTY", 3)) return METER_VENDOR_ETC;

    return METER_VENDOR_UNKNOWN;
}

const char *ConvManufacturerCode(const char *mcode, char *szManufact)
{
    BYTE mask = 0x1F;
    WORD wval = 0;

    if(!mcode || !szManufact) return "";

    memcpy(&wval, mcode, 2);

    szManufact[0] = ((wval >> 10) & mask) + 64;
    szManufact[1] = ((wval >> 5) & mask) + 64;
    szManufact[2] = (wval & mask)+ 64;
    szManufact[3] = 0x00;

    return (const char *)szManufact;
}

/** AiMiR Unit Type을 String 형태로 변환해 준다.
 *
 */
const char * GetUnitStr(BYTE nUnit)
{
    switch(nUnit) {
        case 1:         return "a";         // year
        case 2:         return "mo";        // month
        case 3:         return "wk";        // week
        case 4:         return "d";         // day
        case 5:         return "h";         // hour
        case 6:         return "min";       // min
        case 7:         return "s";         // second
        case 8:         return "degree";    // degree
        case 9:         return "dc";        // degree-celsius
        case 10:        return "currency";  // 
        case 11:        return "m";         // meter
        case 12:        return "m/s";       // meter/sec
        case 13:                            // cubic meter
        case 14:        return "m3";        // cubic meter
        case 15:                            // cubic meter/hour
        case 16:        return "m3/h";      // cubic meter/hour
        case 17:                            // 
        case 18:        return "m3/d";      // 
        case 19:        return "l";         // litre
        case 20:        return "kg";        // kilogram
        case 21:        return "N";         // newton
        case 22:        return "Nm";        // newtonmeter
        case 23:        return "Pa";        // pascal
        case 24:        return "bar";       // bar
        case 25:        return "J";         // joule
        case 26:        return "J/h";       // joule/hour
        case 27:        return "W";         // watt
        case 28:        return "VA";        // volt-ampere
        case 29:        return "var";       // var
        case 30:        return "Wh";        // watt-hour
        case 31:        return "VAh";       // volt-ampere-hour
        case 32:        return "varh";      // var-hour-ampere-hour
        case 33:        return "A";         // ampere
        case 34:        return "C";         // coulomb
        case 35:        return "V";         // volt
        case 36:        return "V/m";       // volt per meter
        case 37:        return "F";         // farad
        case 38:        return "ohm";       // ohm
        case 39:        return "ohm2/m";    //
        case 40:        return "Wb";        // weber
        case 41:        return "T";         // tesla
        case 42:        return "A/m";       // ampere/metre
        case 43:        return "H";         // henry
        case 44:        return "Hz";        // hertz
        case 45:        return "1/(Wh)";    // 
        case 46:        return "1/(varh)";  //
        case 47:        return "1/(VAh)";   //
        case 48:        return "V2h";       // volt-squared-hours
        case 49:        return "A2h";       // ampere-squared-hours
        case 50:        return "kg/s";      // kilogram/sec
        case 51:        return "S,mho";     // siemens
        case 52:        return "K";         // kelvin
        case 53:        return "1/(V2h)";   //
        case 54:        return "1/(A2h)";   //
        case 55:        return "1/m3";      //
        case 56:        return "%";         //
        case 57:        return "Ah";        // ampere-hour
        case 60:        return "Wh/m3";     //
        case 61:        return "J/m3";      //
        case 62:        return "Mol%";      // mole percent
        case 63:        return "g/m3";      //
        case 64:        return "Pa.s";      // pascal second
    }
    return "";
}

/** AiMiR Channel Type String.
 */
const char * GetChannelStr(BYTE nChType)
{
    switch(nChType)
    {
        case  1 : return "Sum Li Active power+ (QI+QIV)";
        case  2 : return "Sum Li Active power- (QII+QIII)";
        case  3 : return "Sum Li Reactive power+ (QI+QII)";
        case  4 : return "Sum Li Reactive power- (QIII+QIV)";
        case  5 : return "Sum Li Reactive power QI";
        case  6 : return "Sum Li Reactive power QII";
        case  7 : return "Sum Li Reactive power QIII";
        case  8 : return "Sum Li Reactive power QIV";
        case  9 : return "Sum Li Apparent power+ (QI+QIV)";
        case 10 : return "Sum Li Apparent power- (QII+QIII)";
        case 11 : return "Current: any phase";
        case 12 : return "Voltage: any phase";
        case 13 : return "Sum Li Power factor";
        case 14 : return "Supply frequency";
        case 15 : return "Sum Li Active power (abs(QI+QIV)+abs(QII+QIII))";
        case 16 : return "Sum Li Active power (abs(QI+QIV)-abs(QII+QIII))";
        case 17 : return "Sum Li Active power QI";
        case 18 : return "Sum Li Active power QII";
        case 19 : return "Sum Li Active power QIII";
        case 20 : return "Sum Li Active power QIV";
        case 21 : return "L1 Active power+ (QI+QIV)";
        case 22 : return "L1 Active power- (QII+QIII)";
        case 23 : return "L1 Reactive power+ (QI+QII)";
        case 24 : return "L1 Reactive power- (QIII+QIV)";
        case 25 : return "L1 Reactive power QI";
        case 26 : return "L1 Reactive power QII";
        case 27 : return "L1 Reactive power QIII";
        case 28 : return "L1 Reactive power QIV";
        case 29 : return "L1 Apparent power+ (QI+QIV)";
        case 30 : return "L1 Apparent power- (QII+QIII)";
        case 31 : return "L1 Current ";
        case 32 : return "L1 Voltage";
        case 33 : return "L1 Power factor";
        case 34 : return "L1 Supply frequency";
        case 35 : return "L1 Active power (abs(QI+QIV)+abs(QII+QIII))";
        case 36 : return "L1 Active power (abs(QI+QIV)-abs(QII+QIII))";
        case 37 : return "L1 Active power QI";
        case 38 : return "L1 Active power QII";
        case 39 : return "L1 Active power QIII";
        case 40 : return "L1 Active power QIV";
        case 41 : return "L2 Active power+ (QI+QIV)";
        case 42 : return "L2 Active power- (QII+QIII)";
        case 43 : return "L2 Reactive power+ (QI+QII)";
        case 44 : return "L2 Reactive power- (QIII+QIV)";
        case 45 : return "L2 Reactive power QI";
        case 46 : return "L2 Reactive power QII";
        case 47 : return "L2 Reactive power QIII";
        case 48 : return "L2 Reactive power QIV";
        case 49 : return "L2 Apparent power+ (QI+QIV)";
        case 50 : return "L2 Apparent power- (QII+QIII)";
        case 51 : return "L2 Current ";
        case 52 : return "L2 Voltage";
        case 53 : return "L2 Power factor";
        case 54 : return "L2 Supply frequency";
        case 55 : return "L2 Active power (abs(QI+QIV)+abs(QII+QIII))";
        case 56 : return "L2 Active power (abs(QI+QIV)-abs(QI+QIII))";
        case 57 : return "L2 Active power QI";
        case 58 : return "L2 Active power QII";
        case 59 : return "L2 Active power QIII";
        case 60 : return "L2 Active power QIV";
        case 61 : return "L3 Active power+ (QI+QIV)";
        case 62 : return "L3 Active power- (QII+QIII)";
        case 63 : return "L3 Reactive power+ (QI+QII)";
        case 64 : return "L3 Reactive power- (QIII+QIV)";
        case 65 : return "L3 Reactive power QI";
        case 66 : return "L3 Reactive power QII";
        case 67 : return "L3 Reactive power QIII";
        case 68 : return "L3 Reactive power QIV";
        case 69 : return "L3 Apparent power+ (QI+QIV)";
        case 70 : return "L3 Apparent power- (QII+QIII)";
        case 71 : return "L3 Current ";
        case 72 : return "L3 Voltage";
        case 73 : return "L3 Power factor";
        case 74 : return "L3 Supply frequency";
        case 75 : return "L3 Active power (abs(QI+QIV)+abs(QII+QIII))";
        case 76 : return "L3 Active power (abs(QI+QIV)-abs(QI+QIII))";
        case 77 : return "L3 Active power QI";
        case 78 : return "L3 Active power QII";
        case 79 : return "L3 Active power QIII";
        case 80 : return "L3 Active power QIV";
        case 81 : return "Angles";
        case 82 : return "Unitless quantities (pulses or pieces)";
        case 84 : return "Sum Li Power factor-";
        case 85 : return "L1 Power factor-";
        case 86 : return "L2 Power factor-";
        case 87 : return "L3 Power factor-";
        case 88 : return "Sum Li A2h QI+QII+QIII+QIV";
        case 89 : return "Sum Li V2h QI+QII+QIII+QIV";
        case 90 : return "SLi current (algebraic sum of the – unsigned – value of the currents in all phases)";
        case 91 : return "Lo Current (neutral)";
        case 92 : return "Lo Voltage (neutral)";
    }
    return "";
}

