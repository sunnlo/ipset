/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

#include <check.h>
#include <glib.h>
#include <glib/gstdio.h>

#include <ipset/bdd/nodes.h>


/*-----------------------------------------------------------------------
 * Temporary file helper
 */

#define TEMP_FILE_TEMPLATE "/tmp/bdd-XXXXXX"

typedef struct _GTempFile
{
    gchar  *filename;
    FILE  *stream;
    GMappedFile  *mapped;
} GTempFile;


static GTempFile *
g_temp_file_new(const gchar *template)
{
    GTempFile  *temp_file = g_slice_new(GTempFile);
    temp_file->filename = g_strdup(template);
    temp_file->stream = NULL;
    temp_file->mapped = NULL;
    return temp_file;
}


static void
g_temp_file_free(GTempFile *temp_file)
{
    g_unlink(temp_file->filename);
    g_free(temp_file->filename);

    if (temp_file->stream != NULL)
    {
        fclose(temp_file->stream);
    }

    if (temp_file->mapped != NULL)
    {
        g_mapped_file_free(temp_file->mapped);
    }
}


static void
g_temp_file_open_stream(GTempFile *temp_file)
{
    int  fd = g_mkstemp(temp_file->filename);
    temp_file->stream = fdopen(fd, "rb+");
}


static void
g_temp_file_open_mapped(GTempFile *temp_file)
{
    if (temp_file->stream != NULL)
    {
        fclose(temp_file->stream);
        temp_file->stream = NULL;
    }
    temp_file->mapped =
        g_mapped_file_new(temp_file->filename, TRUE, NULL);
}


/*-----------------------------------------------------------------------
 * Bit arrays
 */

START_TEST(test_bit_get)
{
    guint16  a = GUINT16_TO_BE(0x6012); /* 0110 0000 0001 0010 */

    fail_unless(IPSET_BIT_GET(&a,  0) == 0,
                "Bit 0 is incorrect");
    fail_unless(IPSET_BIT_GET(&a,  1) == 1,
                "Bit 1 is incorrect");
    fail_unless(IPSET_BIT_GET(&a,  2) == 1,
                "Bit 2 is incorrect");
    fail_unless(IPSET_BIT_GET(&a,  3) == 0,
                "Bit 3 is incorrect");

    fail_unless(IPSET_BIT_GET(&a,  4) == 0,
                "Bit 4 is incorrect");
    fail_unless(IPSET_BIT_GET(&a,  5) == 0,
                "Bit 5 is incorrect");
    fail_unless(IPSET_BIT_GET(&a,  6) == 0,
                "Bit 6 is incorrect");
    fail_unless(IPSET_BIT_GET(&a,  7) == 0,
                "Bit 7 is incorrect");

    fail_unless(IPSET_BIT_GET(&a,  8) == 0,
                "Bit 8 is incorrect");
    fail_unless(IPSET_BIT_GET(&a,  9) == 0,
                "Bit 9 is incorrect");
    fail_unless(IPSET_BIT_GET(&a, 10) == 0,
                "Bit 10 is incorrect");
    fail_unless(IPSET_BIT_GET(&a, 11) == 1,
                "Bit 11 is incorrect");

    fail_unless(IPSET_BIT_GET(&a, 12) == 0,
                "Bit 12 is incorrect");
    fail_unless(IPSET_BIT_GET(&a, 13) == 0,
                "Bit 13 is incorrect");
    fail_unless(IPSET_BIT_GET(&a, 14) == 1,
                "Bit 14 is incorrect");
    fail_unless(IPSET_BIT_GET(&a, 15) == 0,
                "Bit 15 is incorrect");
}
END_TEST


START_TEST(test_bit_set)
{
    guint16  a = 0xffff;        /* 0110 0000 0001 0010 */

    IPSET_BIT_SET(&a,  0, 0);
    IPSET_BIT_SET(&a,  1, 1);
    IPSET_BIT_SET(&a,  2, 1);
    IPSET_BIT_SET(&a,  3, 0);

    IPSET_BIT_SET(&a,  4, 0);
    IPSET_BIT_SET(&a,  5, 0);
    IPSET_BIT_SET(&a,  6, 0);
    IPSET_BIT_SET(&a,  7, 0);

    IPSET_BIT_SET(&a,  8, 0);
    IPSET_BIT_SET(&a,  9, 0);
    IPSET_BIT_SET(&a, 10, 0);
    IPSET_BIT_SET(&a, 11, 1);

    IPSET_BIT_SET(&a, 12, 0);
    IPSET_BIT_SET(&a, 13, 0);
    IPSET_BIT_SET(&a, 14, 1);
    IPSET_BIT_SET(&a, 15, 0);

    fail_unless(GUINT16_TO_BE(0x6012) == a,
                "Incorrect bit result: 0x%04" G_GUINT16_FORMAT,
                a);
}
END_TEST


/*-----------------------------------------------------------------------
 * BDD terminals
 */

START_TEST(test_bdd_false_terminal)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);

    fail_unless(ipset_node_get_type(n_false) == IPSET_TERMINAL_NODE,
                "False terminal has wrong type");

    fail_unless(ipset_terminal_value(n_false) == FALSE,
                "False terminal has wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_true_terminal)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    fail_unless(ipset_node_get_type(n_true) == IPSET_TERMINAL_NODE,
                "True terminal has wrong type");

    fail_unless(ipset_terminal_value(n_true) == TRUE,
                "True terminal has wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_terminal_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  node1 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  node2 =
        ipset_node_cache_terminal(cache, FALSE);

    fail_unless(node1 == node2,
                "Terminal node isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * BDD non-terminals
 */

START_TEST(test_bdd_nonterminal_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);

    fail_unless(ipset_node_get_type(node) == IPSET_NONTERMINAL_NODE,
                "Nonterminal has wrong type");

    ipset_node_t  *n =
        ipset_nonterminal_node(node);

    fail_unless(n->variable == 0,
                "Nonterminal has wrong variable");
    fail_unless(n->low == n_false,
                "Nonterminal has wrong low pointer");
    fail_unless(n->high == n_true,
                "Nonterminal has wrong high pointer");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_nonterminal_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * If we create nonterminals via a BDD engine, they will be
     * reduced — i.e., every nonterminal with the same value will be
     * in the same memory location.
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  node2 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);

    fail_unless(node1 == node2,
                "Nonterminal node isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_nonterminal_reduced_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * We shouldn't have a nonterminal whose low and high subtrees are
     * equal.
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);

    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_false);

    fail_unless(node == n_false,
                "Nonterminal node isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Evaluation
 */

START_TEST(test_bdd_evaluate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = ¬x[0]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);

    /*
     * And test we can get the right results out of it.
     */

    guint8  input1[] = { 0x80 }; /* { TRUE } */
    gboolean  expected1 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bit_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    guint8  input2[] = { 0x00 }; /* { FALSE } */
    gboolean  expected2 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bit_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_evaluate_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = ¬x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, node1, n_false);

    /*
     * And test we can get the right results out of it.
     */

    gboolean  input1[] = { TRUE, TRUE };
    gboolean  expected1 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    gboolean  input2[] = { TRUE, FALSE };
    gboolean  expected2 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    gboolean  input3[] = { FALSE, TRUE };
    gboolean  expected3 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input3)
                == expected3,
                "BDD evaluates to wrong value");

    gboolean  input4[] = { FALSE, FALSE };
    gboolean  expected4 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input4)
                == expected4,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Operators
 */

START_TEST(test_bdd_and_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false0 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true0 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node00 =
        ipset_node_cache_nonterminal(cache, 0, n_false0, n_true0);
    ipset_node_id_t  node01 =
        ipset_node_cache_nonterminal(cache, 1, n_false0, n_true0);
    ipset_node_id_t  node0 =
        ipset_node_cache_and(cache, node00, node01);

    /*
     * And then do it again.
     */

    ipset_node_id_t  n_false1 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true1 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node10 =
        ipset_node_cache_nonterminal(cache, 0, n_false1, n_true1);
    ipset_node_id_t  node11 =
        ipset_node_cache_nonterminal(cache, 1, n_false1, n_true1);
    ipset_node_id_t  node1 =
        ipset_node_cache_and(cache, node10, node11);

    /*
     * Verify that we get the same physical node both times.
     */

    fail_unless(node0 == node1,
                "AND operator result isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_and_evaluate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node =
        ipset_node_cache_and(cache, node0, node1);

    /*
     * And test we can get the right results out of it.
     */

    gboolean  input1[] = { TRUE, TRUE };
    gboolean  expected1 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    gboolean  input2[] = { TRUE, FALSE };
    gboolean  expected2 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    gboolean  input3[] = { FALSE, TRUE };
    gboolean  expected3 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input3)
                == expected3,
                "BDD evaluates to wrong value");

    gboolean  input4[] = { FALSE, FALSE };
    gboolean  expected4 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input4)
                == expected4,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_or_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∨ x[1]
     */

    ipset_node_id_t  n_false0 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true0 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node00 =
        ipset_node_cache_nonterminal(cache, 0, n_false0, n_true0);
    ipset_node_id_t  node01 =
        ipset_node_cache_nonterminal(cache, 1, n_false0, n_true0);
    ipset_node_id_t  node0 =
        ipset_node_cache_or(cache, node00, node01);

    /*
     * And then do it again.
     */

    ipset_node_id_t  n_false1 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true1 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node10 =
        ipset_node_cache_nonterminal(cache, 0, n_false1, n_true1);
    ipset_node_id_t  node11 =
        ipset_node_cache_nonterminal(cache, 1, n_false1, n_true1);
    ipset_node_id_t  node1 =
        ipset_node_cache_or(cache, node10, node11);

    /*
     * Verify that we get the same physical node both times.
     */

    fail_unless(node0 == node1,
                "OR operator result isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_or_evaluate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∨ x[1]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, node0, node1);

    /*
     * And test we can get the right results out of it.
     */

    gboolean  input1[] = { TRUE, TRUE };
    gboolean  expected1 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    gboolean  input2[] = { TRUE, FALSE };
    gboolean  expected2 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    gboolean  input3[] = { FALSE, TRUE };
    gboolean  expected3 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input3)
                == expected3,
                "BDD evaluates to wrong value");

    gboolean  input4[] = { FALSE, FALSE };
    gboolean  expected4 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input4)
                == expected4,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_ite_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = IF (x[0] ∧ x[1])
     *          THEN (2)
     *          ELSE (0)
     */

    ipset_node_id_t  n_false0 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true0 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node00 =
        ipset_node_cache_nonterminal(cache, 0, n_false0, n_true0);
    ipset_node_id_t  node01 =
        ipset_node_cache_nonterminal(cache, 1, n_false0, n_true0);
    ipset_node_id_t  node02 =
        ipset_node_cache_and(cache, node00, node01);

    ipset_node_id_t  n_zero0 =
        ipset_node_cache_terminal(cache, 0);
    ipset_node_id_t  n_two0 =
        ipset_node_cache_terminal(cache, 2);

    ipset_node_id_t  node0 =
        ipset_node_cache_ite(cache, node02, n_two0, n_zero0);

    /*
     * And then do it again.
     */

    ipset_node_id_t  n_false1 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true1 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node10 =
        ipset_node_cache_nonterminal(cache, 0, n_false1, n_true1);
    ipset_node_id_t  node11 =
        ipset_node_cache_nonterminal(cache, 1, n_false1, n_true1);
    ipset_node_id_t  node12 =
        ipset_node_cache_and(cache, node10, node11);

    ipset_node_id_t  n_zero1 =
        ipset_node_cache_terminal(cache, 0);
    ipset_node_id_t  n_two1 =
        ipset_node_cache_terminal(cache, 2);

    ipset_node_id_t  node1 =
        ipset_node_cache_ite(cache, node12, n_two1, n_zero1);

    /*
     * Verify that we get the same physical node both times.
     */

    fail_unless(node0 == node1,
                "ITE operator result isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_ite_evaluate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = IF (x[0] ∧ x[1])
     *          THEN (2)
     *          ELSE (0)
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node2 =
        ipset_node_cache_and(cache, node0, node1);

    ipset_node_id_t  n_zero =
        ipset_node_cache_terminal(cache, 0);
    ipset_node_id_t  n_two =
        ipset_node_cache_terminal(cache, 2);

    ipset_node_id_t  node =
        ipset_node_cache_ite(cache, node2, n_two, n_zero);

    /*
     * And test we can get the right results out of it.
     */

    gboolean  input1[] = { TRUE, TRUE };
    gint  expected1 = 2;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    gboolean  input2[] = { TRUE, FALSE };
    gint  expected2 = 0;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    gboolean  input3[] = { FALSE, TRUE };
    gint  expected3 = 0;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input3)
                == expected3,
                "BDD evaluates to wrong value");

    gboolean  input4[] = { FALSE, FALSE };
    gint  expected4 = 0;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input4)
                == expected4,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Memory size
 */

START_TEST(test_bdd_size_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  t0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  f0 =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);
    ipset_node_id_t  t1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  t2 =
        ipset_node_cache_nonterminal(cache, 2, n_false, n_true);

    ipset_node_id_t  n1 =
        ipset_node_cache_and(cache, t0, t1);
    ipset_node_id_t  n2 =
        ipset_node_cache_and(cache, f0, t2);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, n1, n2);

    /*
     * And verify how big it is.
     */

    fail_unless(ipset_node_reachable_count(node) == 3u,
                "BDD has wrong number of nodes");

    fail_unless(ipset_node_memory_size(node) ==
                3u * sizeof(ipset_node_t),
                "BDD takes up wrong amount of space");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Serialization
 */

START_TEST(test_bdd_save_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = TRUE
     */

    ipset_node_id_t  node =
        ipset_node_cache_terminal(cache, TRUE);

    /*
     * Serialize the BDD into a string.
     */

    GTempFile  *temp_file = g_temp_file_new(TEMP_FILE_TEMPLATE);
    g_temp_file_open_stream(temp_file);

    fail_unless(ipset_node_cache_save(temp_file->stream, cache, node, NULL),
                "Cannot serialize BDD");

    const char  *raw_expected =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x18"   // length
        "\x00\x00\x00\x00"                   // node count
        "\x00\x00\x00\x01"                   // terminal value
        ;
    const size_t  expected_length = 24;

    g_temp_file_open_mapped(temp_file);
    gpointer  buf = g_mapped_file_get_contents(temp_file->mapped);
    gsize  len = g_mapped_file_get_length(temp_file->mapped);

    fail_unless(expected_length == len,
                "Serialized BDD has wrong length "
                "(expected %zu, got %zu)",
                expected_length, len);

    fail_unless(memcmp(raw_expected, buf, expected_length) == 0,
                "Serialized BDD has incorrect data");

    g_temp_file_free(temp_file);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_save_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  t0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  f0 =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);
    ipset_node_id_t  t1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  t2 =
        ipset_node_cache_nonterminal(cache, 2, n_false, n_true);

    ipset_node_id_t  n1 =
        ipset_node_cache_and(cache, t0, t1);
    ipset_node_id_t  n2 =
        ipset_node_cache_and(cache, f0, t2);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, n1, n2);

    /*
     * Serialize the BDD into a string.
     */

    GTempFile  *temp_file = g_temp_file_new(TEMP_FILE_TEMPLATE);
    g_temp_file_open_stream(temp_file);

    fail_unless(ipset_node_cache_save(temp_file->stream, cache, node, NULL),
                "Cannot serialize BDD");

    const char  *raw_expected =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x2f"   // length
        "\x00\x00\x00\x03"                   // node count
        // node -1
        "\x02"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -2
        "\x01"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -3
        "\x00"                               // variable
        "\xff\xff\xff\xff"                   // low
        "\xff\xff\xff\xfe"                   // high
        ;
    const size_t  expected_length = 47;

    g_temp_file_open_mapped(temp_file);
    gpointer  buf = g_mapped_file_get_contents(temp_file->mapped);
    gsize  len = g_mapped_file_get_length(temp_file->mapped);

    fail_unless(expected_length == len,
                "Serialized BDD has wrong length "
                "(expected %zu, got %zu)",
                expected_length, len);

    fail_unless(memcmp(raw_expected, buf, expected_length) == 0,
                "Serialized BDD has incorrect data");

    g_temp_file_free(temp_file);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_load_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = TRUE
     */

    ipset_node_id_t  node =
        ipset_node_cache_terminal(cache, TRUE);

    /*
     * Read a BDD from a string.
     */

    const char  *raw =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x18"   // length
        "\x00\x00\x00\x00"                   // node count
        "\x00\x00\x00\x01"                   // terminal value
        ;
    const size_t  raw_length = 24;

    GTempFile  *temp_file = g_temp_file_new(TEMP_FILE_TEMPLATE);
    g_temp_file_open_stream(temp_file);
    fwrite(raw, raw_length, 1, temp_file->stream);
    fflush(temp_file->stream);
    fseek(temp_file->stream, 0, SEEK_SET);

    GError  *error = NULL;
    ipset_node_id_t  read =
        ipset_node_cache_load(temp_file->stream, cache, &error);

    fail_unless(error == NULL,
                "Error reading BDD from stream");

    fail_unless(read == node,
                "BDD from stream doesn't match expected");

    g_temp_file_free(temp_file);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_load_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  t0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  f0 =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);
    ipset_node_id_t  t1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  t2 =
        ipset_node_cache_nonterminal(cache, 2, n_false, n_true);

    ipset_node_id_t  n1 =
        ipset_node_cache_and(cache, t0, t1);
    ipset_node_id_t  n2 =
        ipset_node_cache_and(cache, f0, t2);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, n1, n2);

    /*
     * Read a BDD from a string.
     */

    const char  *raw =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x2f"   // length
        "\x00\x00\x00\x03"                   // node count
        // node -1
        "\x02"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -2
        "\x01"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -3
        "\x00"                               // variable
        "\xff\xff\xff\xff"                   // low
        "\xff\xff\xff\xfe"                   // high
        ;
    const size_t  raw_length = 47;

    GTempFile  *temp_file = g_temp_file_new(TEMP_FILE_TEMPLATE);
    g_temp_file_open_stream(temp_file);
    fwrite(raw, raw_length, 1, temp_file->stream);
    fflush(temp_file->stream);
    fseek(temp_file->stream, 0, SEEK_SET);

    GError  *error = NULL;
    ipset_node_id_t  read =
        ipset_node_cache_load(temp_file->stream, cache, &error);

    fail_unless(error == NULL,
                "Error reading BDD from stream");

    fail_unless(read == node,
                "BDD from stream doesn't match expected");

    g_temp_file_free(temp_file);
    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Iteration
 */

START_TEST(test_bdd_iterate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = ¬x[0]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);

    /*
     * And test that iterating the BDD gives us the expected results.
     */

    ipset_assignment_t  *expected;
    expected = ipset_assignment_new();

    ipset_bdd_iterator_t  *it = ipset_node_iterate(node);

    fail_if(it->finished,
            "Iterator should not be empty");
    ipset_assignment_clear(expected);
    ipset_assignment_set(expected, 0, IPSET_FALSE);
    fail_unless(ipset_assignment_equal(expected, it->assignment),
                "Iterator assignment 0 doesn't match");
    fail_unless(TRUE == it->value,
                "Iterator value 0 doesn't match");

    ipset_bdd_iterator_advance(it);
    fail_if(it->finished,
            "Iterator should have more than 1 element");
    ipset_assignment_clear(expected);
    ipset_assignment_set(expected, 0, IPSET_TRUE);
    fail_unless(ipset_assignment_equal(expected, it->assignment),
                "Iterator assignment 1 doesn't match");
    fail_unless(FALSE == it->value,
                "Iterator value 1 doesn't match (%u)", it->value);

    ipset_bdd_iterator_advance(it);
    fail_unless(it->finished,
                "Iterator should have 2 elements");

    ipset_assignment_free(expected);
    ipset_bdd_iterator_free(it);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_iterate_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = ¬x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, node1, n_false);

    /*
     * And test that iterating the BDD gives us the expected results.
     */

    ipset_assignment_t  *expected;
    expected = ipset_assignment_new();

    ipset_bdd_iterator_t  *it = ipset_node_iterate(node);

    fail_if(it->finished,
            "Iterator should not be empty");
    ipset_assignment_clear(expected);
    ipset_assignment_set(expected, 0, IPSET_FALSE);
    ipset_assignment_set(expected, 1, IPSET_FALSE);
    fail_unless(ipset_assignment_equal(expected, it->assignment),
                "Iterator assignment 0 doesn't match");
    fail_unless(FALSE == it->value,
                "Iterator value 0 doesn't match");

    ipset_bdd_iterator_advance(it);
    fail_if(it->finished,
            "Iterator should have more than 1 element");
    ipset_assignment_clear(expected);
    ipset_assignment_set(expected, 0, IPSET_FALSE);
    ipset_assignment_set(expected, 1, IPSET_TRUE);
    fail_unless(ipset_assignment_equal(expected, it->assignment),
                "Iterator assignment 1 doesn't match");
    fail_unless(TRUE == it->value,
                "Iterator value 1 doesn't match (%u)", it->value);

    ipset_bdd_iterator_advance(it);
    fail_if(it->finished,
            "Iterator should have more than 2 elements");
    ipset_assignment_clear(expected);
    ipset_assignment_set(expected, 0, IPSET_TRUE);
    fail_unless(ipset_assignment_equal(expected, it->assignment),
                "Iterator assignment 2 doesn't match");
    fail_unless(FALSE == it->value,
                "Iterator value 2 doesn't match (%u)", it->value);

    ipset_bdd_iterator_advance(it);
    fail_unless(it->finished,
                "Iterator should have 3 elements");

    ipset_assignment_free(expected);
    ipset_bdd_iterator_free(it);
    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

static Suite *
test_suite()
{
    Suite  *s = suite_create("bdd");

    TCase  *tc_bits = tcase_create("bits");
    tcase_add_test(tc_bits, test_bit_get);
    tcase_add_test(tc_bits, test_bit_set);
    suite_add_tcase(s, tc_bits);

    TCase  *tc_terminals = tcase_create("terminals");
    tcase_add_test(tc_terminals, test_bdd_false_terminal);
    tcase_add_test(tc_terminals, test_bdd_true_terminal);
    tcase_add_test(tc_terminals, test_bdd_terminal_reduced_1);
    suite_add_tcase(s, tc_terminals);

    TCase  *tc_nonterminals = tcase_create("nonterminals");
    tcase_add_test(tc_nonterminals, test_bdd_nonterminal_1);
    tcase_add_test(tc_nonterminals, test_bdd_nonterminal_reduced_1);
    tcase_add_test(tc_nonterminals, test_bdd_nonterminal_reduced_2);
    suite_add_tcase(s, tc_nonterminals);

    TCase  *tc_evaluation = tcase_create("evaluation");
    tcase_add_test(tc_evaluation, test_bdd_evaluate_1);
    tcase_add_test(tc_evaluation, test_bdd_evaluate_2);
    suite_add_tcase(s, tc_evaluation);

    TCase  *tc_operators = tcase_create("operators");
    tcase_add_test(tc_operators, test_bdd_and_reduced_1);
    tcase_add_test(tc_operators, test_bdd_and_evaluate_1);
    tcase_add_test(tc_operators, test_bdd_or_reduced_1);
    tcase_add_test(tc_operators, test_bdd_or_evaluate_1);
    tcase_add_test(tc_operators, test_bdd_ite_reduced_1);
    tcase_add_test(tc_operators, test_bdd_ite_evaluate_1);
    suite_add_tcase(s, tc_operators);

    TCase  *tc_size = tcase_create("size");
    tcase_add_test(tc_size, test_bdd_size_1);
    suite_add_tcase(s, tc_size);

    TCase  *tc_serialization = tcase_create("serialization");
    tcase_add_test(tc_serialization, test_bdd_save_1);
    tcase_add_test(tc_serialization, test_bdd_save_2);
    tcase_add_test(tc_serialization, test_bdd_load_1);
    tcase_add_test(tc_serialization, test_bdd_load_2);
    suite_add_tcase(s, tc_serialization);

    TCase  *tc_iteration = tcase_create("iteration");
    tcase_add_test(tc_iteration, test_bdd_iterate_1);
    tcase_add_test(tc_iteration, test_bdd_iterate_2);
    suite_add_tcase(s, tc_iteration);

    Suite  *s1 = suite_create("bdd");
    TCase  *tc1 = tcase_create("bits");
    tcase_add_test(tc1, test_bdd_save_1);
    suite_add_tcase(s1, tc1);
    (void) s;

    return s1;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
