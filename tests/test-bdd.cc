/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */


#include <iostream>
#include <sstream>

// Both of the following try to define CHECK.  We want the one from
// UnitTest.
#include <glog/logging.h>
#undef CHECK
#include <UnitTest++.h>

#include <ip/utils.hh>
#include <ip/bdd/engine.hh>
#include <ip/bdd/nodes.hh>

using namespace ip::bdd;


//--------------------------------------------------------------------
// BDD terminals

TEST(BDD_False_Terminal)
{
    std::cerr << "Starting BDD_False_Terminal test case." << std::endl;

    node_cache_t  cache;

    node_id_t  n_false = cache.terminal(false);

    CHECK_EQUAL(false, n_false);
}

TEST(BDD_True_Terminal)
{
    std::cerr << "Starting BDD_True_Terminal test case." << std::endl;

    node_cache_t  cache;

    node_id_t  n_true = cache.terminal(true);

    CHECK_EQUAL(true, n_true);
}


TEST(BDD_Terminal_Reduced_1)
{
    std::cerr << "Starting BDD_Terminal_Reduced_1 test case." << std::endl;

    // If we create terminals via a BDD engine, they will be reduced —
    // i.e., every terminal with the same value will be in the same
    // memory location.

    node_cache_t  cache;

    node_id_t  node1 = cache.terminal(false);
    node_id_t  node2 = cache.terminal(false);

    CHECK_EQUAL(node1, node2);
}


//--------------------------------------------------------------------
// BDD non-terminals

TEST(BDD_Nonterminal_1)
{
    std::cerr << "Starting BDD_Nonterminal_1 test case." << std::endl;

    node_cache_t  cache;

    node_id_t  n_false = 0;
    node_id_t  n_true = 1;

    node_id_t  node =
        cache.nonterminal(0, n_false, n_true);

    const node_t  &n = cache.node(node);

    CHECK_EQUAL(0u, n.variable());
    CHECK_EQUAL(n_false, n.low());
    CHECK_EQUAL(n_true, n.high());
}


TEST(BDD_Nonterminal_Reduced_1)
{
    std::cerr << "Starting BDD_Nonterminal_Reduced_1 test case." << std::endl;

    // If we create nonterminals via a BDD engine, they will be
    // reduced — i.e., every nonterminal with the same value will be
    // in the same memory location.

    node_cache_t  cache;

    node_id_t  n_false = cache.terminal(false);
    node_id_t  n_true = cache.terminal(true);

    node_id_t  node1 =
        cache.nonterminal(0, n_false, n_true);
    node_id_t  node2 =
        cache.nonterminal(0, n_false, n_true);

    CHECK_EQUAL(node1, node2);
}


TEST(BDD_Nonterminal_Reduced_2)
{
    std::cerr << "Starting BDD_Nonterminal_Reduced_2 test case." << std::endl;

    // We shouldn't have a nonterminal whose low and high subtrees are
    // equal.

    node_cache_t  cache;

    node_id_t  n_false = cache.terminal(false);

    node_id_t  node =
        cache.nonterminal(0, n_false, n_false);

    CHECK_EQUAL(node, n_false);
}


//--------------------------------------------------------------------
// Evaluation

TEST(BDD_Evaluate_1)
{
    std::cerr << "Starting BDD_Evaluate_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = ¬x[0]

    node_cache_t  cache;

    node_id_t  n_false = cache.terminal(false);
    node_id_t  n_true = cache.terminal(true);

    node_id_t  node = cache.nonterminal(0, n_true, n_false);

    // And test we can get the right results out of it.

    bool  input1[] = { true };
    bool  input2[] = { false };

    CHECK_EQUAL(false, cache.evaluate(node, input1));
    CHECK_EQUAL(true, cache.evaluate(node, input2));
}


TEST(BDD_Evaluate_2)
{
    std::cerr << "Starting BDD_Evaluate_2 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = ¬x[0] ∧ x[1]

    node_cache_t  cache;

    node_id_t  n_false = cache.terminal(false);
    node_id_t  n_true = cache.terminal(true);

    node_id_t  node1 = cache.nonterminal(1, n_false, n_true);
    node_id_t  node = cache.nonterminal(0, node1, n_false);

    // And test we can get the right results out of it.

    bool  input1[] = { true, true };
    bool  input2[] = { true, false };
    bool  input3[] = { false, true };
    bool  input4[] = { false, false };

    CHECK_EQUAL(false, cache.evaluate(node, input1));
    CHECK_EQUAL(false, cache.evaluate(node, input2));
    CHECK_EQUAL(true, cache.evaluate(node, input3));
    CHECK_EQUAL(false, cache.evaluate(node, input4));
}


//--------------------------------------------------------------------
// Operators

TEST(BDD_And_Reduced_1)
{
    std::cerr << "Starting BDD_And_Reduced_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = x[0] ∧ x[1]

    bool_engine_t  engine;

    node_id_t  n_false0 = engine.false_node();
    node_id_t  n_true0 = engine.true_node();

    node_id_t  node00 = engine.nonterminal(0, n_false0, n_true0);
    node_id_t  node01 = engine.nonterminal(1, n_false0, n_true0);
    node_id_t  node0 = engine.apply_and(node00, node01);

    // And then do it again.

    node_id_t  n_false1 = engine.false_node();
    node_id_t  n_true1 = engine.true_node();

    node_id_t  node10 = engine.nonterminal(0, n_false1, n_true1);
    node_id_t  node11 = engine.nonterminal(1, n_false1, n_true1);
    node_id_t  node1 = engine.apply_and(node10, node11);

    // Verify that we get the same physical node both times.

    CHECK_EQUAL(node0, node1);
}


TEST(BDD_And_Evaluate_1)
{
    std::cerr << "Starting BDD_And_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = x[0] ∧ x[1]

    bool_engine_t  engine;

    node_id_t  n_false = engine.false_node();
    node_id_t  n_true = engine.true_node();

    node_id_t  node0 = engine.nonterminal(0, n_false, n_true);
    node_id_t  node1 = engine.nonterminal(1, n_false, n_true);
    node_id_t  node = engine.apply_and(node0, node1);

    // And test we can get the right results out of it.

    bool  input1[] = { true, true };
    bool  input2[] = { true, false };
    bool  input3[] = { false, true };
    bool  input4[] = { false, false };

    CHECK_EQUAL(true, engine.evaluate(node, input1));
    CHECK_EQUAL(false, engine.evaluate(node, input2));
    CHECK_EQUAL(false, engine.evaluate(node, input3));
    CHECK_EQUAL(false, engine.evaluate(node, input4));
}


TEST(BDD_Or_Reduced_1)
{
    std::cerr << "Starting BDD_Or_Reduced_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = x[0] ∧ x[1]

    bool_engine_t  engine;

    node_id_t  n_false0 = engine.false_node();
    node_id_t  n_true0 = engine.true_node();

    node_id_t  node00 = engine.nonterminal(0, n_false0, n_true0);
    node_id_t  node01 = engine.nonterminal(1, n_false0, n_true0);
    node_id_t  node0 = engine.apply_or(node00, node01);

    // And then do it again.

    node_id_t  n_false1 = engine.false_node();
    node_id_t  n_true1 = engine.true_node();

    node_id_t  node10 = engine.nonterminal(0, n_false1, n_true1);
    node_id_t  node11 = engine.nonterminal(1, n_false1, n_true1);
    node_id_t  node1 = engine.apply_or(node10, node11);

    // Verify that we get the same physical node both times.

    CHECK_EQUAL(node0, node1);
}


TEST(BDD_Or_Evaluate_1)
{
    std::cerr << "Starting BDD_Or_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = x[0] ∧ x[1]

    bool_engine_t  engine;

    node_id_t  n_false = engine.false_node();
    node_id_t  n_true = engine.true_node();

    node_id_t  node0 = engine.nonterminal(0, n_false, n_true);
    node_id_t  node1 = engine.nonterminal(1, n_false, n_true);
    node_id_t  node = engine.apply_or(node0, node1);

    // And test we can get the right results out of it.

    bool  input1[] = { true, true };
    bool  input2[] = { true, false };
    bool  input3[] = { false, true };
    bool  input4[] = { false, false };

    CHECK_EQUAL(true, engine.evaluate(node, input1));
    CHECK_EQUAL(true, engine.evaluate(node, input2));
    CHECK_EQUAL(true, engine.evaluate(node, input3));
    CHECK_EQUAL(false, engine.evaluate(node, input4));
}


//--------------------------------------------------------------------
// Memory size

TEST(BDD_Size_1)
{
    std::cerr << "Starting BDD_Size_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])

    bool_engine_t  engine;

    node_id_t  n_false = engine.false_node();
    node_id_t  n_true = engine.true_node();

    node_id_t  t0 = engine.nonterminal(0, n_false, n_true);
    node_id_t  f0 = engine.nonterminal(0, n_true, n_false);
    node_id_t  t1 = engine.nonterminal(1, n_false, n_true);
    node_id_t  t2 = engine.nonterminal(2, n_false, n_true);

    node_id_t  n1 = engine.apply_and(t0, t1);
    node_id_t  n2 = engine.apply_and(f0, t2);
    node_id_t  node = engine.apply_or(n1, n2);

    // And verify how big it is.

    CHECK_EQUAL(3u, engine.reachable_node_count(node));
    CHECK_EQUAL(3u * sizeof(node_t), engine.memory_size(node));
}


//--------------------------------------------------------------------
// Serialization

TEST(BDD_Save_1)
{
    std::cerr << "Starting BDD_Save_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = TRUE

    bool_engine_t  engine;

    node_id_t  node = engine.true_node();

    // Serialize the BDD into a string stream.

    std::ostringstream  ss;
    engine.save(ss, node);

    const char  *raw_expected =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x18"   // length
        "\x00\x00\x00\x00"                   // node count
        "\x00\x00\x00\x01"                   // terminal value
        ;
    const size_t  expected_length = 24;
    const std::string  expected(raw_expected, expected_length);

    CHECK_EQUAL(UnitTest::binary_string(expected),
                UnitTest::binary_string(ss.str()));
}

TEST(BDD_Save_2)
{
    std::cerr << "Starting BDD_Save_2 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])

    bool_engine_t  engine;

    node_id_t  n_false = engine.false_node();
    node_id_t  n_true = engine.true_node();

    node_id_t  t0 = engine.nonterminal(0, n_false, n_true);
    node_id_t  f0 = engine.nonterminal(0, n_true, n_false);
    node_id_t  t1 = engine.nonterminal(1, n_false, n_true);
    node_id_t  t2 = engine.nonterminal(2, n_false, n_true);

    node_id_t  n1 = engine.apply_and(t0, t1);
    node_id_t  n2 = engine.apply_and(f0, t2);
    node_id_t  node = engine.apply_or(n1, n2);

    // Serialize the BDD into a string stream.

    std::ostringstream  ss;
    engine.save(ss, node);

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
    const std::string  expected(raw_expected, expected_length);

    CHECK_EQUAL(UnitTest::binary_string(expected),
                UnitTest::binary_string(ss.str()));
}

TEST(BDD_Load_1)
{
    std::cerr << "Starting BDD_Load_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])

    bool_engine_t  engine;

    node_id_t  n_false = engine.false_node();
    node_id_t  n_true = engine.true_node();

    node_id_t  t0 = engine.nonterminal(0, n_false, n_true);
    node_id_t  f0 = engine.nonterminal(0, n_true, n_false);
    node_id_t  t1 = engine.nonterminal(1, n_false, n_true);
    node_id_t  t2 = engine.nonterminal(2, n_false, n_true);

    node_id_t  n1 = engine.apply_and(t0, t1);
    node_id_t  n2 = engine.apply_and(f0, t2);
    node_id_t  node = engine.apply_or(n1, n2);

    // Read a BDD from a string stream.

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
    const std::string  raw_str(raw, raw_length);
    std::istringstream  ss(raw_str);

    node_id_t  read;
    CHECK(engine.load(ss, read));

    CHECK_EQUAL(node, read);
}

TEST(BDD_Load_2)
{
    std::cerr << "Starting BDD_Load_2 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = TRUE

    bool_engine_t  engine;

    node_id_t  node = engine.true_node();

    // Read a BDD from a string stream.

    const char  *raw =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x18"   // length
        "\x00\x00\x00\x00"                   // node count
        "\x00\x00\x00\x01"                   // terminal value
        ;
    const size_t  raw_length = 24;
    const std::string  raw_str(raw, raw_length);
    std::istringstream  ss(raw_str);

    node_id_t  read;
    CHECK(engine.load(ss, read));

    CHECK_EQUAL(node, read);
}


//--------------------------------------------------------------------
// Iteration

TEST(BDD_Iterate_1)
{
    std::cerr << "Starting BDD_Iterate_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = ¬x[0]

    node_cache_t  cache;

    node_id_t  n_false = cache.terminal(false);
    node_id_t  n_true = cache.terminal(true);

    node_id_t  node = cache.nonterminal(0, n_true, n_false);

    // And test we can get the right iterated results out of it.

    assignment_t  a;

    node_cache_t::iterator  it = cache.begin(node);

    a.clear();
    a[0] = false;
    CHECK_EQUAL(a, it->first);
    CHECK_EQUAL(true, it->second);

    ++it;
    a.clear();
    a[0] = true;
    CHECK_EQUAL(a, it->first);
    CHECK_EQUAL(false, it->second);

    ++it;
    CHECK(it == cache.end());
}


TEST(BDD_Iterate_2)
{
    std::cerr << "Starting BDD_Iterate_2 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = ¬x[0] ∧ x[1]

    node_cache_t  cache;

    node_id_t  n_false = cache.terminal(false);
    node_id_t  n_true = cache.terminal(true);

    node_id_t  node1 = cache.nonterminal(1, n_false, n_true);
    node_id_t  node = cache.nonterminal(0, node1, n_false);

    // And test we can get the right iterated results out of it.

    assignment_t  a;

    node_cache_t::iterator  it = cache.begin(node);

    a.clear();
    a[0] = false;
    a[1] = false;
    CHECK_EQUAL(a, it->first);
    CHECK_EQUAL(false, it->second);

    ++it;
    a.clear();
    a[0] = false;
    a[1] = true;
    CHECK_EQUAL(a, it->first);
    CHECK_EQUAL(true, it->second);

    ++it;
    a.clear();
    a[0] = true;
    CHECK_EQUAL(a, it->first);
    CHECK_EQUAL(false, it->second);

    ++it;
    CHECK(it == cache.end());
}


//--------------------------------------------------------------------
// Boilerplate

int main(int argc, char **argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
    return UnitTest::RunAllTests();
}
