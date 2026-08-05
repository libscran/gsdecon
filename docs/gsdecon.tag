<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>blocked.hpp</name>
    <path>gsdecon/</path>
    <filename>blocked_8hpp.html</filename>
    <includes id="Options_8hpp" name="Options.hpp" local="yes" import="no" module="no" objc="no">Options.hpp</includes>
    <includes id="Results_8hpp" name="Results.hpp" local="yes" import="no" module="no" objc="no">Results.hpp</includes>
    <namespace>gsdecon</namespace>
  </compound>
  <compound kind="file">
    <name>compute.hpp</name>
    <path>gsdecon/</path>
    <filename>compute_8hpp.html</filename>
    <includes id="Options_8hpp" name="Options.hpp" local="yes" import="no" module="no" objc="no">Options.hpp</includes>
    <includes id="Results_8hpp" name="Results.hpp" local="yes" import="no" module="no" objc="no">Results.hpp</includes>
    <namespace>gsdecon</namespace>
  </compound>
  <compound kind="file">
    <name>gsdecon.hpp</name>
    <path>gsdecon/</path>
    <filename>gsdecon_8hpp.html</filename>
    <includes id="compute_8hpp" name="compute.hpp" local="yes" import="no" module="no" objc="no">compute.hpp</includes>
    <includes id="blocked_8hpp" name="blocked.hpp" local="yes" import="no" module="no" objc="no">blocked.hpp</includes>
    <includes id="Results_8hpp" name="Results.hpp" local="yes" import="no" module="no" objc="no">Results.hpp</includes>
    <includes id="Options_8hpp" name="Options.hpp" local="yes" import="no" module="no" objc="no">Options.hpp</includes>
    <namespace>gsdecon</namespace>
  </compound>
  <compound kind="file">
    <name>Options.hpp</name>
    <path>gsdecon/</path>
    <filename>Options_8hpp.html</filename>
    <class kind="struct">gsdecon::Options</class>
    <namespace>gsdecon</namespace>
  </compound>
  <compound kind="file">
    <name>Results.hpp</name>
    <path>gsdecon/</path>
    <filename>Results_8hpp.html</filename>
    <class kind="struct">gsdecon::Buffers</class>
    <class kind="struct">gsdecon::Results</class>
    <namespace>gsdecon</namespace>
  </compound>
  <compound kind="struct">
    <name>gsdecon::Buffers</name>
    <filename>structgsdecon_1_1Buffers.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="variable">
      <type>Float_ *</type>
      <name>scores</name>
      <anchorfile>structgsdecon_1_1Buffers.html</anchorfile>
      <anchor>a1cb8723f84dbfd6eb01aed2eb9df3a79</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Float_ *</type>
      <name>weights</name>
      <anchorfile>structgsdecon_1_1Buffers.html</anchorfile>
      <anchor>a5a8bf5d0934551d54bc8f1168776c294</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>gsdecon::Options</name>
    <filename>structgsdecon_1_1Options.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>rank</name>
      <anchorfile>structgsdecon_1_1Options.html</anchorfile>
      <anchor>a294f5013812f89fa2ab72baa6c30124e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>scale</name>
      <anchorfile>structgsdecon_1_1Options.html</anchorfile>
      <anchor>aeda0448b282093b21643717c1d615c09</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>scran_blocks::WeightPolicy</type>
      <name>block_weight_policy</name>
      <anchorfile>structgsdecon_1_1Options.html</anchorfile>
      <anchor>a3be2392bd4ffa5c24ff509dd745120d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>scran_blocks::VariableWeightParameters</type>
      <name>variable_block_weight_parameters</name>
      <anchorfile>structgsdecon_1_1Options.html</anchorfile>
      <anchor>a5e6570013997204acf154c31e5a62108</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structgsdecon_1_1Options.html</anchorfile>
      <anchor>ab8605d018810bc716fae33bd97995e60</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>realize_matrix</name>
      <anchorfile>structgsdecon_1_1Options.html</anchorfile>
      <anchor>a0d6e41781464027402301047c62918e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>irlba::Options&lt; Eigen::VectorXd &gt;</type>
      <name>irlba_options</name>
      <anchorfile>structgsdecon_1_1Options.html</anchorfile>
      <anchor>a777c653ebda6fff2ac1a9c2df270b718</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>gsdecon::Results</name>
    <filename>structgsdecon_1_1Results.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Float_ &gt;</type>
      <name>scores</name>
      <anchorfile>structgsdecon_1_1Results.html</anchorfile>
      <anchor>ab6c32ea41976a14f96f630bd24e57759</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Float_ &gt;</type>
      <name>weights</name>
      <anchorfile>structgsdecon_1_1Results.html</anchorfile>
      <anchor>a63423d8b7e2cb1c48444ea6c3f3205c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>irlba::Metrics</type>
      <name>metrics</name>
      <anchorfile>structgsdecon_1_1Results.html</anchorfile>
      <anchor>ad59a851b5462073b39bd890104e3bd1d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>gsdecon</name>
    <filename>namespacegsdecon.html</filename>
    <class kind="struct">gsdecon::Buffers</class>
    <class kind="struct">gsdecon::Options</class>
    <class kind="struct">gsdecon::Results</class>
    <member kind="function">
      <type>irlba::Metrics</type>
      <name>compute_blocked</name>
      <anchorfile>namespacegsdecon.html</anchorfile>
      <anchor>af9961ba28f8f0ed804921c853ed8be17</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;matrix, const Block_ *const block, const std::size_t num_blocks, const Options &amp;options, const Buffers&lt; Float_ &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>Results&lt; Float_ &gt;</type>
      <name>compute_blocked</name>
      <anchorfile>namespacegsdecon.html</anchorfile>
      <anchor>af1eafd6168233853317ede2a135d2799</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;matrix, const Block_ *const block, const std::size_t num_blocks, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>irlba::Metrics</type>
      <name>compute</name>
      <anchorfile>namespacegsdecon.html</anchorfile>
      <anchor>ae6adfd257ba561edd4e01ba004756ad1</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;matrix, const Options &amp;options, const Buffers&lt; Float_ &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>Results&lt; Float_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacegsdecon.html</anchorfile>
      <anchor>a99f95f15512b37823a4dfe21847008b4</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;matrix, const Options &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>C++ port of the GSDecon algorithm</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
