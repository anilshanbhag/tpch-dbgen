/* generate flat files for data load */
#include <stdio.h>
#ifndef VMS
#include <sys/types.h>
#endif
#if defined(SUN)
#include <unistd.h>
#endif
#include <math.h>

#include "dss.h"
#include <string.h>
#include <fstream>
using namespace std;

/*
 * Function Prototypes
 */
FILE *print_prep (int table, int update);
int pr_drange (int tbl, int64_t min, int64_t cnt, long num);

void print_prep(int table, int update, ofstream& f) {
  char upath[128];
  FILE *res;

  if (updates) {
    if (update > 0) /* updates */
      if (insert_segments) {
        int this_segment;
        if (strcmp(tdefs[table].name, "orders.tbl"))
          this_segment = ++insert_orders_segment;
        else
          this_segment = ++insert_lineitem_segment;
        sprintf(upath, "%s%c%s.u%d.%d", env_config(PATH_TAG, PATH_DFLT),
                PATH_SEP, tdefs[table].name, update % 10000, this_segment);
      } else {
        sprintf(upath, "%s%c%s.u%d", env_config(PATH_TAG, PATH_DFLT), PATH_SEP,
                tdefs[table].name, update);
      }
    else /* deletes */
        if (delete_segments) {
      ++delete_segment;
      sprintf(upath, "%s%cdelete.u%d.%d", env_config(PATH_TAG, PATH_DFLT),
              PATH_SEP, -update % 10000, delete_segment);
    } else {
      sprintf(upath, "%s%cdelete.%d", env_config(PATH_TAG, PATH_DFLT), PATH_SEP,
              -update);
    }
    f.open(upath);
    if (!f.is_open()) {
      fprintf(stderr, "Open failed for %s at %s:%d\n",
              upath, __FILE__, __LINE__);
      exit(1);
    }
  }

  tbl_open(table, f);
}

int pr_cust(table_t *tp, int mode) {
  static ofstream fp;
  customer_t *c = static_cast<customer_t *>(tp);

  if (!fp.is_open())
    print_prep(CUST, 0, fp);

  PR_STRT(fp);
  PR_HUGE(fp, c->custkey);
  if (scale <= 3000) {
    PR_VSTR(fp, c->name, C_NAME_LEN);
  } else {
    PR_VSTR(fp, c->name, C_NAME_LEN + 3);
  }
  PR_VSTR(fp, c->address, c->alen);
  PR_HUGE(fp, c->nation_code);
  PR_STR(fp, c->phone, PHONE_LEN);
  PR_MONEY(fp, c->acctbal);
  PR_STR(fp, c->mktsegment, C_MSEG_LEN);
  PR_VSTR_LAST(fp, c->comment, c->clen);
  PR_END(fp);

  return (0);
}

/*
 * print the numbered order
 */
int pr_order(table_t *tp, int mode) {
  order_t *o = static_cast<order_t *>(tp);
  static ofstream fp_o;
  static int last_mode = 0;

  if (!fp_o.is_open() || mode != last_mode) {
    if (fp_o.is_open())
      fp_o.close();
    print_prep(ORDER, mode, fp_o);
    last_mode = mode;
  }
  PR_STRT(fp_o);
  PR_HUGE(fp_o, o->okey);
  PR_HUGE(fp_o, o->custkey);
  PR_CHR(fp_o, o->orderstatus);
  PR_MONEY(fp_o, o->totalprice);
  PR_STR(fp_o, o->odate, DATE_LEN);
  PR_STR(fp_o, o->opriority, O_OPRIO_LEN);
  PR_STR(fp_o, o->clerk, O_CLRK_LEN);
  PR_INT(fp_o, o->spriority);
  PR_VSTR_LAST(fp_o, o->comment, o->clen);
  PR_END(fp_o);

  return (0);
}

/*
 * print an order's lineitems
 */
int pr_line(table_t *tp, int mode) {
  order_t *o = static_cast<order_t *>(tp);
  static ofstream fp_l;
  static int last_mode = 0;
  long i;

  if (!fp_l.is_open() || mode != last_mode) {
    if (fp_l.is_open())
      fp_l.close();
    print_prep(LINE, mode, fp_l);
    last_mode = mode;
  }

  for (i = 0; i < o->lines; i++) {
    PR_STRT(fp_l);
    PR_HUGE(fp_l, o->l[i].okey);
    PR_HUGE(fp_l, o->l[i].partkey);
    PR_HUGE(fp_l, o->l[i].suppkey);
    PR_HUGE(fp_l, o->l[i].lcnt);
    PR_HUGE(fp_l, o->l[i].quantity);
    PR_MONEY(fp_l, o->l[i].eprice);
    PR_MONEY(fp_l, o->l[i].discount);
    PR_MONEY(fp_l, o->l[i].tax);
    PR_CHR(fp_l, o->l[i].rflag[0]);
    PR_CHR(fp_l, o->l[i].lstatus[0]);
    PR_STR(fp_l, o->l[i].sdate, DATE_LEN);
    PR_STR(fp_l, o->l[i].cdate, DATE_LEN);
    PR_STR(fp_l, o->l[i].rdate, DATE_LEN);
    PR_STR(fp_l, o->l[i].shipinstruct, L_INST_LEN);
    PR_STR(fp_l, o->l[i].shipmode, L_SMODE_LEN);
    PR_VSTR_LAST(fp_l, o->l[i].comment, o->l[i].clen);
    PR_END(fp_l);
  }

  return (0);
}

/*
 * print the numbered order *and* its associated lineitems
 */
int pr_order_line(table_t *tp, int mode) {
  order_t *o = static_cast<order_t *>(tp);
  tdefs[ORDER].name = tdefs[ORDER_LINE].name;
  pr_order(o, mode);
  pr_line(o, mode);

  return (0);
}

/*
 * print the given part
 */
int pr_part(table_t *tp, int mode) {
  part_t *part = static_cast<part_t *>(tp);
  static ofstream p_fp;

  if (!p_fp.is_open())
    print_prep(PART, 0, p_fp);

  PR_STRT(p_fp);
  PR_HUGE(p_fp, part->partkey);
  PR_VSTR(p_fp, part->name, part->nlen);
  PR_STR(p_fp, part->mfgr, P_MFG_LEN);
  PR_STR(p_fp, part->brand, P_BRND_LEN);
  PR_VSTR(p_fp, part->type, part->tlen);
  PR_HUGE(p_fp, part->size);
  PR_STR(p_fp, part->container, P_CNTR_LEN);
  PR_MONEY(p_fp, part->retailprice);
  PR_VSTR_LAST(p_fp, part->comment, part->clen);
  PR_END(p_fp);

  return (0);
}

/*
 * print the given part's suppliers
 */
int pr_psupp(table_t *tp, int mode) {
  part_t *part = static_cast<part_t *>(tp);
  static ofstream ps_fp;
  long i;

  if (!ps_fp.is_open())
    print_prep(PSUPP, mode, ps_fp);

  for (i = 0; i < SUPP_PER_PART; i++) {
    PR_STRT(ps_fp);
    PR_HUGE(ps_fp, part->s[i].partkey);
    PR_HUGE(ps_fp, part->s[i].suppkey);
    PR_HUGE(ps_fp, part->s[i].qty);
    PR_MONEY(ps_fp, part->s[i].scost);
    PR_VSTR_LAST(ps_fp, part->s[i].comment, part->s[i].clen);
    PR_END(ps_fp);
  }

  return (0);
}

/*
 * print the given part *and* its suppliers
 */
int pr_part_psupp(table_t *tp, int mode) {
  part_t *part = static_cast<part_t *>(tp);
  tdefs[PART].name = tdefs[PART_PSUPP].name;
  pr_part(part, mode);
  pr_psupp(part, mode);

  return (0);
}

int pr_supp(table_t *tp, int mode) {
  supplier_t *supp = static_cast<supplier_t *>(tp);
  static ofstream fp;

  if (!fp.is_open())
    print_prep(SUPP, mode, fp);

  PR_STRT(fp);
  PR_HUGE(fp, supp->suppkey);
  PR_STR(fp, supp->name, S_NAME_LEN);
  PR_VSTR(fp, supp->address, supp->alen);
  PR_HUGE(fp, supp->nation_code);
  PR_STR(fp, supp->phone, PHONE_LEN);
  PR_MONEY(fp, supp->acctbal);
  PR_VSTR_LAST(fp, supp->comment, supp->clen);
  PR_END(fp);

  return (0);
}

int pr_nation(table_t *tp, int mode) {
  code_t *c = static_cast<code_t *>(tp);
  static ofstream fp;

  if (!fp.is_open())
    print_prep(NATION, mode, fp);

  PR_STRT(fp);
  PR_HUGE(fp, c->code);
  PR_STR(fp, c->text, NATION_LEN);
  PR_INT(fp, c->join);
  PR_VSTR_LAST(fp, c->comment, c->clen);
  PR_END(fp);

  return (0);
}

int pr_region(table_t *tp, int mode) {
  code_t *c = static_cast<code_t *>(tp);
  static ofstream fp;

  if (!fp.is_open())
    print_prep(REGION, mode, fp);

  PR_STRT(fp);
  PR_HUGE(fp, c->code);
  PR_STR(fp, c->text, REGION_LEN);
  PR_VSTR_LAST(fp, c->comment, c->clen);
  PR_END(fp);

  return (0);
}

/*
 * NOTE: this routine does NOT use the BCD2_* routines. As a result,
 * it WILL fail if the keys being deleted exceed 32 bits. Since this
 * would require ~660 update iterations, this seems an acceptable
 * oversight
 */
int pr_drange(int tbl, int64_t min, int64_t cnt, long num) {
  static int last_num = 0;
  static ofstream dfp;
  int64_t child = -1;
  int64_t start, last, current;

  static int64_t rows_per_segment = 0;
  static int64_t rows_this_segment = 0;

  if (last_num != num) {
    if (dfp.is_open())
      dfp.close();
    print_prep(tbl, -num, dfp);
    if (!dfp.is_open())
      return (-1);
    last_num = num;
    rows_this_segment = 0;
  }

  start = MK_SPARSE(min, num / (10000 / UPD_PCT));
  last = start - 1;
  for (child = min; cnt > 0; child++, cnt--) {
    current = MK_SPARSE(child, num / (10000 / UPD_PCT));
    if (delete_segments) {

      if (rows_per_segment == 0)
        rows_per_segment = (cnt / delete_segments) + 1;
      if ((++rows_this_segment) > rows_per_segment) {
        dfp.close();
        print_prep(tbl, -num, dfp);
        if (!dfp.is_open())
          return (-1);
        last_num = num;
        rows_this_segment = 1;
      }
    }
    PR_STRT(dfp);
    PR_HUGE(dfp, current);
    PR_END(dfp);
    start = current;
    last = current;
  }

  return (0);
}
