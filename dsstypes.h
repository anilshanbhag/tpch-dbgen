#pragma once

#include "shared.h"

/*
 * general definitions and control information for the DSS data types
 * and function prototypes
 */


/*
 * typedefs
 */
struct customer_t: table_t
{
    int64_t            custkey;
    char            name[C_NAME_LEN + 3];
    char            address[C_ADDR_MAX + 1];
    int             alen;
    int64_t            nation_code;
    char            phone[PHONE_LEN + 1];
    int64_t            acctbal;
    char            mktsegment[MAXAGG_LEN + 1];
    char            comment[C_CMNT_MAX + 1];
    int             clen;
};

/* customers.c */
long mk_cust   (int64_t n_cust, customer_t * c);
int pr_cust    (table_t* c, int mode);
int ld_cust    (table_t* c, int mode);

struct line_t: table_t
{
    int64_t	    okey;
    int64_t            partkey;
    int64_t            suppkey;
    int64_t            lcnt;
    int64_t            quantity;
    int64_t            eprice;
    int64_t            discount;
    int64_t            tax;
    char            rflag[1];
    char            lstatus[1];
    char            cdate[DATE_LEN];
    char            sdate[DATE_LEN];
    char            rdate[DATE_LEN];
    char           shipinstruct[MAXAGG_LEN + 1];
    char           shipmode[MAXAGG_LEN + 1];
    char           comment[L_CMNT_MAX + 1];
    int            clen;
};

struct order_t: table_t
{
    int64_t	    okey;
    int64_t        custkey;
    char            orderstatus;
    int64_t            totalprice;
    char            odate[DATE_LEN];
    char            opriority[MAXAGG_LEN + 1];
    char            clerk[O_CLRK_LEN + 1];
    long            spriority;
    int64_t            lines;
    char            comment[O_CMNT_MAX + 1];
    int            clen;
    line_t          l[O_LCNT_MAX];
};

/* order.c */
long	mk_order	(int64_t index, order_t * o, long upd_num);
int		pr_order	(table_t* o, int mode);
int		ld_order	(table_t* o, int mode);
void	mk_sparse	(int64_t index, int64_t *ok, long seq);

struct partsupp_t: table_t
{
    int64_t            partkey;
    int64_t            suppkey;
    int64_t            qty;
    int64_t            scost;
    char           comment[PS_CMNT_MAX + 1];
    int            clen;
};

struct part_t: table_t
{
    int64_t           partkey;
    char           name[P_NAME_LEN + 1];
    int            nlen;
    char           mfgr[P_MFG_LEN + 1];
    char           brand[P_BRND_LEN + 1];
    char           type[P_TYPE_LEN + 1];
    int            tlen;
    int64_t           size;
    char           container[P_CNTR_LEN + 1];
    int64_t           retailprice;
    char           comment[P_CMNT_MAX + 1];
    int            clen;
    partsupp_t     s[SUPP_PER_PART];
};

/* parts.c */
long mk_part   (int64_t index, part_t * p);
int pr_part    (table_t* part, int mode);
int ld_part    (table_t* part, int mode);

struct supplier_t: table_t
{
    int64_t            suppkey;
    char            name[S_NAME_LEN + 1];
    char            address[S_ADDR_MAX + 1];
    int             alen;
    int64_t            nation_code;
    char            phone[PHONE_LEN + 1];
    int64_t            acctbal;
    char            comment[S_CMNT_MAX + 1];
    int             clen;
};

/* supplier.c */
long mk_supp   (int64_t index, supplier_t * s);
int pr_supp    (table_t* supp, int mode);
int ld_supp    (table_t* supp, int mode);

struct dss_time_t
{
    int64_t            timekey;
    char            alpha[DATE_LEN];
    long            year;
    long            month;
    long            week;
    long            day;
};

/* time.c */
long mk_time   (int64_t h, dss_time_t * t);

/*
 * this assumes that N_CMNT_LEN >= R_CMNT_LEN
 */
struct code_t: table_t
{
    int64_t            code;
    char            *text;
    long            join;
    char            comment[N_CMNT_MAX + 1];
    int             clen;
};

/* code table */
int mk_nation   (int64_t i, code_t * c);
int pr_nation    (table_t* c, int mode);
int ld_nation    (table_t* c, int mode);
int mk_region   (int64_t i, code_t * c);
int pr_region    (table_t* c, int mode);
int ld_region    (table_t* c, int mode);

