


<!DOCTYPE html>
<html>
  <head>
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="chrome=1">
        <title>src/KSegmentor/src/KSegmentor.cpp at KSlice3D from kfritscher/kslice - GitHub</title>
    <link rel="search" type="application/opensearchdescription+xml" href="/opensearch.xml" title="GitHub" />
    <link rel="fluid-icon" href="https://github.com/fluidicon.png" title="GitHub" />

    
    

    <meta content="authenticity_token" name="csrf-param" />
<meta content="b36630ef7e23b9dcd7bd67f8b73cc561b40f026a" name="csrf-token" />

    <link href="https://a248.e.akamai.net/assets.github.com/stylesheets/bundles/github-9bef515c140b863f61df966f44e62caaa19ccae1.css" media="screen" rel="stylesheet" type="text/css" />
    

    <script src="https://a248.e.akamai.net/assets.github.com/javascripts/bundles/jquery-0afaa9d8025004af7fc6f2a561837057d3f21298.js" type="text/javascript"></script>
    <script src="https://a248.e.akamai.net/assets.github.com/javascripts/bundles/github-c3520f89f16a33cc97a93889496732ddc0d6c49b.js" type="text/javascript"></script>
    

      <link rel='permalink' href='/kfritscher/kslice/blob/13d71926258ea390923b4c7c12b84ab35735723e/src/KSegmentor/src/KSegmentor.cpp'>
    <META NAME="ROBOTS" CONTENT="NOINDEX, FOLLOW">

    <meta name="description" content="kslice - interactive medical image processing" />
  <link href="https://github.com/kfritscher/kslice/commits/KSlice3D.atom" rel="alternate" title="Recent Commits to kslice:KSlice3D" type="application/atom+xml" />

  </head>


  <body class="logged_out page-blob  env-production ">
    


    

    <div id="main">
      <div id="header" class="true">
          <a class="logo" href="https://github.com">
            <img alt="github" class="default svg" height="45" src="https://a248.e.akamai.net/assets.github.com/images/modules/header/logov6.svg" />
            <img alt="github" class="default png" height="45" src="https://a248.e.akamai.net/assets.github.com/images/modules/header/logov6.png" />
            <!--[if (gt IE 8)|!(IE)]><!-->
            <img alt="github" class="hover svg" height="45" src="https://a248.e.akamai.net/assets.github.com/images/modules/header/logov6-hover.svg" />
            <img alt="github" class="hover png" height="45" src="https://a248.e.akamai.net/assets.github.com/images/modules/header/logov6-hover.png" />
            <!--<![endif]-->
          </a>

        <div class="topsearch">
    <!--
      make sure to use fully qualified URLs here since this nav
      is used on error pages on other domains
    -->
    <ul class="nav logged_out">
        <li class="pricing"><a href="https://github.com/plans">Signup and Pricing</a></li>
        <li class="explore"><a href="https://github.com/explore">Explore GitHub</a></li>
      <li class="features"><a href="https://github.com/features">Features</a></li>
        <li class="blog"><a href="https://github.com/blog">Blog</a></li>
      <li class="login"><a href="https://github.com/login?return_to=%2Fkfritscher%2Fkslice%2Fblob%2FKSlice3D%2Fsrc%2FKSegmentor%2Fsrc%2FKSegmentor.cpp">Login</a></li>
    </ul>
</div>

      </div>

      
            <div class="site">
      <div class="pagehead repohead vis-public fork  instapaper_ignore readability-menu">


      <div class="title-actions-bar">
        <h1>
          <a href="/kfritscher">kfritscher</a> /
          <strong><a href="/kfritscher/kslice" class="js-current-repository">kslice</a></strong>
            <span class="fork-flag">
              <span class="text">forked from <a href="/pkarasev3/kslice">pkarasev3/kslice</a></span>
            </span>
        </h1>
        



            <ul class="pagehead-actions">

        <li class="js-toggler-container watch-button-container ">
          <a href="/kfritscher/kslice/toggle_watch" class="minibutton btn-watch watch-button js-toggler-target" data-method="post" data-remote="true"><span><span class="icon"></span>Watch</span></a>
          <a href="/kfritscher/kslice/toggle_watch" class="minibutton btn-watch unwatch-button js-toggler-target" data-method="post" data-remote="true"><span><span class="icon"></span>Unwatch</span></a>
        </li>
            <li><a href="/kfritscher/kslice/fork" class="minibutton btn-fork fork-button" data-method="post"><span><span class="icon"></span>Fork</span></a></li>

      <li class="repostats">
        <ul class="repo-stats">
          <li class="watchers ">
            <a href="/kfritscher/kslice/watchers" title="Watchers" class="tooltipped downwards">
              1
            </a>
          </li>
          <li class="forks">
            <a href="/kfritscher/kslice/network" title="Forks" class="tooltipped downwards">
              2
            </a>
          </li>
        </ul>
      </li>
    </ul>

      </div>

        

  <ul class="tabs">
    <li><a href="/kfritscher/kslice/tree/KSlice3D" class="selected" highlight="repo_sourcerepo_downloadsrepo_commitsrepo_tagsrepo_branches">Code</a></li>
    <li><a href="/kfritscher/kslice/network" highlight="repo_networkrepo_fork_queue">Network</a>
    <li><a href="/kfritscher/kslice/pulls" highlight="repo_pulls">Pull Requests <span class='counter'>0</span></a></li>



    <li><a href="/kfritscher/kslice/graphs" highlight="repo_graphsrepo_contributors">Stats &amp; Graphs</a></li>

  </ul>

  
<div class="frame frame-center tree-finder" style="display:none"
      data-tree-list-url="/kfritscher/kslice/tree-list/13d71926258ea390923b4c7c12b84ab35735723e"
      data-blob-url-prefix="/kfritscher/kslice/blob/13d71926258ea390923b4c7c12b84ab35735723e"
    >

  <div class="breadcrumb">
    <b><a href="/kfritscher/kslice">kslice</a></b> /
    <input class="tree-finder-input" type="text" name="query" autocomplete="off" spellcheck="false">
  </div>

    <div class="octotip">
      <p>
        <a href="/kfritscher/kslice/dismiss-tree-finder-help" class="dismiss js-dismiss-tree-list-help" title="Hide this notice forever">Dismiss</a>
        <strong>Octotip:</strong> You've activated the <em>file finder</em>
        by pressing <span class="kbd">t</span> Start typing to filter the
        file list. Use <span class="kbd badmono">↑</span> and
        <span class="kbd badmono">↓</span> to navigate,
        <span class="kbd">enter</span> to view files.
      </p>
    </div>

  <table class="tree-browser" cellpadding="0" cellspacing="0">
    <tr class="js-header"><th>&nbsp;</th><th>name</th></tr>
    <tr class="js-no-results no-results" style="display: none">
      <th colspan="2">No matching files</th>
    </tr>
    <tbody class="js-results-list">
    </tbody>
  </table>
</div>

<div id="jump-to-line" style="display:none">
  <h2>Jump to Line</h2>
  <form>
    <input class="textfield" type="text">
    <div class="full-button">
      <button type="submit" class="classy">
        <span>Go</span>
      </button>
    </div>
  </form>
</div>


<div class="subnav-bar">

  <ul class="actions">
    
      <li class="switcher">

        <div class="context-menu-container js-menu-container">
          <span class="text">Current branch:</span>
          <a href="#"
             class="minibutton bigger switcher context-menu-button js-menu-target js-commitish-button btn-branch repo-tree"
             data-master-branch="master"
             data-ref="KSlice3D">
            <span><span class="icon"></span>KSlice3D</span>
          </a>

          <div class="context-pane commitish-context js-menu-content">
            <a href="javascript:;" class="close js-menu-close"></a>
            <div class="title">Switch Branches/Tags</div>
            <div class="body pane-selector commitish-selector js-filterable-commitishes">
              <div class="filterbar">
                <div class="placeholder-field js-placeholder-field">
                  <label class="placeholder" for="context-commitish-filter-field" data-placeholder-mode="sticky">Filter branches/tags</label>
                  <input type="text" id="context-commitish-filter-field" class="commitish-filter" />
                </div>

                <ul class="tabs">
                  <li><a href="#" data-filter="branches" class="selected">Branches</a></li>
                  <li><a href="#" data-filter="tags">Tags</a></li>
                </ul>
              </div>

                <div class="commitish-item branch-commitish selector-item">
                  <h4>
                      <a href="/kfritscher/kslice/blob/KSlice3D/src/KSegmentor/src/KSegmentor.cpp" data-name="KSlice3D">KSlice3D</a>
                  </h4>
                </div>
                <div class="commitish-item branch-commitish selector-item">
                  <h4>
                      <a href="/kfritscher/kslice/blob/master/src/KSegmentor/src/KSegmentor.cpp" data-name="master">master</a>
                  </h4>
                </div>


              <div class="no-results" style="display:none">Nothing to show</div>
            </div>
          </div><!-- /.commitish-context-context -->
        </div>

      </li>
  </ul>

  <ul class="subnav">
    <li><a href="/kfritscher/kslice/tree/KSlice3D" class="selected" highlight="repo_source">Files</a></li>
    <li><a href="/kfritscher/kslice/commits/KSlice3D" highlight="repo_commits">Commits</a></li>
    <li><a href="/kfritscher/kslice/branches" class="" highlight="repo_branches">Branches <span class="counter">2</span></a></li>
    <li><a href="/kfritscher/kslice/tags" class="blank" highlight="repo_tags">Tags <span class="counter">0</span></a></li>
    <li><a href="/kfritscher/kslice/downloads" class="blank" highlight="repo_downloads">Downloads <span class="counter">0</span></a></li>
  </ul>

</div>

  
  
  


        

      </div><!-- /.pagehead -->

      




  
  <p class="last-commit">Latest commit to the <strong>KSlice3D</strong> branch</p>

<div class="commit commit-tease js-details-container">
  <p class="commit-title ">
      <a href="/kfritscher/kslice/commit/13d71926258ea390923b4c7c12b84ab35735723e" class="message">MAde some fixes concerning transformations (didi not realize that ima…</a>
      <a href="javascript:;" class="minibutton expander-minibutton js-details-target"><span>…</span></a>
  </p>
    <div class="commit-desc"><pre>…ge isnot displayed correctly when pixelspacing is !=1).However, still image reslicer does not correctly update the image extent after transformations - stilldo not know why...</pre></div>
  <div class="commit-meta">
    <a href="/kfritscher/kslice/commit/13d71926258ea390923b4c7c12b84ab35735723e" class="sha-block">commit <span class="sha">13d7192625</span></a>

    <div class="authorship">
      <img class="gravatar" height="20" src="https://secure.gravatar.com/avatar/5865c5a6773d575cf8f75408536eb967?s=140&amp;d=https://a248.e.akamai.net/assets.github.com%2Fimages%2Fgravatars%2Fgravatar-140.png" width="20" />
      <span class="author-name">kfritscher</span>
      authored <time class="js-relative-date" datetime="2011-11-10T13:47:39-08:00" title="2011-11-10 13:47:39">November 10, 2011</time>

    </div>
  </div>
</div>


  <div id="slider">

    <div class="breadcrumb" data-path="src/KSegmentor/src/KSegmentor.cpp/">
      <b><a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e" class="js-rewrite-sha">kslice</a></b> / <a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e/src" class="js-rewrite-sha">src</a> / <a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e/src/KSegmentor" class="js-rewrite-sha">KSegmentor</a> / <a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e/src/KSegmentor/src" class="js-rewrite-sha">src</a> / KSegmentor.cpp       <span style="display:none" id="clippy_1151" class="clippy-text">src/KSegmentor/src/KSegmentor.cpp</span>
      
      <object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000"
              width="110"
              height="14"
              class="clippy"
              id="clippy" >
      <param name="movie" value="https://a248.e.akamai.net/assets.github.com/flash/clippy.swf?v5"/>
      <param name="allowScriptAccess" value="always" />
      <param name="quality" value="high" />
      <param name="scale" value="noscale" />
      <param NAME="FlashVars" value="id=clippy_1151&amp;copied=copied!&amp;copyto=copy to clipboard">
      <param name="bgcolor" value="#FFFFFF">
      <param name="wmode" value="opaque">
      <embed src="https://a248.e.akamai.net/assets.github.com/flash/clippy.swf?v5"
             width="110"
             height="14"
             name="clippy"
             quality="high"
             allowScriptAccess="always"
             type="application/x-shockwave-flash"
             pluginspage="http://www.macromedia.com/go/getflashplayer"
             FlashVars="id=clippy_1151&amp;copied=copied!&amp;copyto=copy to clipboard"
             bgcolor="#FFFFFF"
             wmode="opaque"
      />
      </object>
      

    </div>

    <div class="frames">
      <div class="frame frame-center" data-path="src/KSegmentor/src/KSegmentor.cpp/" data-permalink-url="/kfritscher/kslice/blob/13d71926258ea390923b4c7c12b84ab35735723e/src/KSegmentor/src/KSegmentor.cpp" data-title="src/KSegmentor/src/KSegmentor.cpp at KSlice3D from kfritscher/kslice - GitHub" data-type="blob">
          <ul class="big-actions">
            <li><a class="file-edit-link minibutton js-rewrite-sha" href="/kfritscher/kslice/edit/13d71926258ea390923b4c7c12b84ab35735723e/src/KSegmentor/src/KSegmentor.cpp" data-method="post"><span>Edit this file</span></a></li>
          </ul>

        <div id="files">
          <div class="file">
            <div class="meta">
              <div class="info">
                <span class="icon"><img alt="Txt" height="16" src="https://a248.e.akamai.net/assets.github.com/images/icons/txt.png" width="16" /></span>
                <span class="mode" title="File Mode">100644</span>
                  <span>429 lines (356 sloc)</span>
                <span>13.895 kb</span>
              </div>
              <ul class="actions">
                <li><a href="/kfritscher/kslice/raw/KSlice3D/src/KSegmentor/src/KSegmentor.cpp" id="raw-url">raw</a></li>
                  <li><a href="/kfritscher/kslice/blame/KSlice3D/src/KSegmentor/src/KSegmentor.cpp">blame</a></li>
                <li><a href="/kfritscher/kslice/commits/KSlice3D/src/KSegmentor/src/KSegmentor.cpp">history</a></li>
              </ul>
            </div>
              <div class="data type-c++">
      <table cellpadding="0" cellspacing="0" class="lines">
        <tr>
          <td>
            <pre class="line_numbers"><span id="L1" rel="#L1">1</span>
<span id="L2" rel="#L2">2</span>
<span id="L3" rel="#L3">3</span>
<span id="L4" rel="#L4">4</span>
<span id="L5" rel="#L5">5</span>
<span id="L6" rel="#L6">6</span>
<span id="L7" rel="#L7">7</span>
<span id="L8" rel="#L8">8</span>
<span id="L9" rel="#L9">9</span>
<span id="L10" rel="#L10">10</span>
<span id="L11" rel="#L11">11</span>
<span id="L12" rel="#L12">12</span>
<span id="L13" rel="#L13">13</span>
<span id="L14" rel="#L14">14</span>
<span id="L15" rel="#L15">15</span>
<span id="L16" rel="#L16">16</span>
<span id="L17" rel="#L17">17</span>
<span id="L18" rel="#L18">18</span>
<span id="L19" rel="#L19">19</span>
<span id="L20" rel="#L20">20</span>
<span id="L21" rel="#L21">21</span>
<span id="L22" rel="#L22">22</span>
<span id="L23" rel="#L23">23</span>
<span id="L24" rel="#L24">24</span>
<span id="L25" rel="#L25">25</span>
<span id="L26" rel="#L26">26</span>
<span id="L27" rel="#L27">27</span>
<span id="L28" rel="#L28">28</span>
<span id="L29" rel="#L29">29</span>
<span id="L30" rel="#L30">30</span>
<span id="L31" rel="#L31">31</span>
<span id="L32" rel="#L32">32</span>
<span id="L33" rel="#L33">33</span>
<span id="L34" rel="#L34">34</span>
<span id="L35" rel="#L35">35</span>
<span id="L36" rel="#L36">36</span>
<span id="L37" rel="#L37">37</span>
<span id="L38" rel="#L38">38</span>
<span id="L39" rel="#L39">39</span>
<span id="L40" rel="#L40">40</span>
<span id="L41" rel="#L41">41</span>
<span id="L42" rel="#L42">42</span>
<span id="L43" rel="#L43">43</span>
<span id="L44" rel="#L44">44</span>
<span id="L45" rel="#L45">45</span>
<span id="L46" rel="#L46">46</span>
<span id="L47" rel="#L47">47</span>
<span id="L48" rel="#L48">48</span>
<span id="L49" rel="#L49">49</span>
<span id="L50" rel="#L50">50</span>
<span id="L51" rel="#L51">51</span>
<span id="L52" rel="#L52">52</span>
<span id="L53" rel="#L53">53</span>
<span id="L54" rel="#L54">54</span>
<span id="L55" rel="#L55">55</span>
<span id="L56" rel="#L56">56</span>
<span id="L57" rel="#L57">57</span>
<span id="L58" rel="#L58">58</span>
<span id="L59" rel="#L59">59</span>
<span id="L60" rel="#L60">60</span>
<span id="L61" rel="#L61">61</span>
<span id="L62" rel="#L62">62</span>
<span id="L63" rel="#L63">63</span>
<span id="L64" rel="#L64">64</span>
<span id="L65" rel="#L65">65</span>
<span id="L66" rel="#L66">66</span>
<span id="L67" rel="#L67">67</span>
<span id="L68" rel="#L68">68</span>
<span id="L69" rel="#L69">69</span>
<span id="L70" rel="#L70">70</span>
<span id="L71" rel="#L71">71</span>
<span id="L72" rel="#L72">72</span>
<span id="L73" rel="#L73">73</span>
<span id="L74" rel="#L74">74</span>
<span id="L75" rel="#L75">75</span>
<span id="L76" rel="#L76">76</span>
<span id="L77" rel="#L77">77</span>
<span id="L78" rel="#L78">78</span>
<span id="L79" rel="#L79">79</span>
<span id="L80" rel="#L80">80</span>
<span id="L81" rel="#L81">81</span>
<span id="L82" rel="#L82">82</span>
<span id="L83" rel="#L83">83</span>
<span id="L84" rel="#L84">84</span>
<span id="L85" rel="#L85">85</span>
<span id="L86" rel="#L86">86</span>
<span id="L87" rel="#L87">87</span>
<span id="L88" rel="#L88">88</span>
<span id="L89" rel="#L89">89</span>
<span id="L90" rel="#L90">90</span>
<span id="L91" rel="#L91">91</span>
<span id="L92" rel="#L92">92</span>
<span id="L93" rel="#L93">93</span>
<span id="L94" rel="#L94">94</span>
<span id="L95" rel="#L95">95</span>
<span id="L96" rel="#L96">96</span>
<span id="L97" rel="#L97">97</span>
<span id="L98" rel="#L98">98</span>
<span id="L99" rel="#L99">99</span>
<span id="L100" rel="#L100">100</span>
<span id="L101" rel="#L101">101</span>
<span id="L102" rel="#L102">102</span>
<span id="L103" rel="#L103">103</span>
<span id="L104" rel="#L104">104</span>
<span id="L105" rel="#L105">105</span>
<span id="L106" rel="#L106">106</span>
<span id="L107" rel="#L107">107</span>
<span id="L108" rel="#L108">108</span>
<span id="L109" rel="#L109">109</span>
<span id="L110" rel="#L110">110</span>
<span id="L111" rel="#L111">111</span>
<span id="L112" rel="#L112">112</span>
<span id="L113" rel="#L113">113</span>
<span id="L114" rel="#L114">114</span>
<span id="L115" rel="#L115">115</span>
<span id="L116" rel="#L116">116</span>
<span id="L117" rel="#L117">117</span>
<span id="L118" rel="#L118">118</span>
<span id="L119" rel="#L119">119</span>
<span id="L120" rel="#L120">120</span>
<span id="L121" rel="#L121">121</span>
<span id="L122" rel="#L122">122</span>
<span id="L123" rel="#L123">123</span>
<span id="L124" rel="#L124">124</span>
<span id="L125" rel="#L125">125</span>
<span id="L126" rel="#L126">126</span>
<span id="L127" rel="#L127">127</span>
<span id="L128" rel="#L128">128</span>
<span id="L129" rel="#L129">129</span>
<span id="L130" rel="#L130">130</span>
<span id="L131" rel="#L131">131</span>
<span id="L132" rel="#L132">132</span>
<span id="L133" rel="#L133">133</span>
<span id="L134" rel="#L134">134</span>
<span id="L135" rel="#L135">135</span>
<span id="L136" rel="#L136">136</span>
<span id="L137" rel="#L137">137</span>
<span id="L138" rel="#L138">138</span>
<span id="L139" rel="#L139">139</span>
<span id="L140" rel="#L140">140</span>
<span id="L141" rel="#L141">141</span>
<span id="L142" rel="#L142">142</span>
<span id="L143" rel="#L143">143</span>
<span id="L144" rel="#L144">144</span>
<span id="L145" rel="#L145">145</span>
<span id="L146" rel="#L146">146</span>
<span id="L147" rel="#L147">147</span>
<span id="L148" rel="#L148">148</span>
<span id="L149" rel="#L149">149</span>
<span id="L150" rel="#L150">150</span>
<span id="L151" rel="#L151">151</span>
<span id="L152" rel="#L152">152</span>
<span id="L153" rel="#L153">153</span>
<span id="L154" rel="#L154">154</span>
<span id="L155" rel="#L155">155</span>
<span id="L156" rel="#L156">156</span>
<span id="L157" rel="#L157">157</span>
<span id="L158" rel="#L158">158</span>
<span id="L159" rel="#L159">159</span>
<span id="L160" rel="#L160">160</span>
<span id="L161" rel="#L161">161</span>
<span id="L162" rel="#L162">162</span>
<span id="L163" rel="#L163">163</span>
<span id="L164" rel="#L164">164</span>
<span id="L165" rel="#L165">165</span>
<span id="L166" rel="#L166">166</span>
<span id="L167" rel="#L167">167</span>
<span id="L168" rel="#L168">168</span>
<span id="L169" rel="#L169">169</span>
<span id="L170" rel="#L170">170</span>
<span id="L171" rel="#L171">171</span>
<span id="L172" rel="#L172">172</span>
<span id="L173" rel="#L173">173</span>
<span id="L174" rel="#L174">174</span>
<span id="L175" rel="#L175">175</span>
<span id="L176" rel="#L176">176</span>
<span id="L177" rel="#L177">177</span>
<span id="L178" rel="#L178">178</span>
<span id="L179" rel="#L179">179</span>
<span id="L180" rel="#L180">180</span>
<span id="L181" rel="#L181">181</span>
<span id="L182" rel="#L182">182</span>
<span id="L183" rel="#L183">183</span>
<span id="L184" rel="#L184">184</span>
<span id="L185" rel="#L185">185</span>
<span id="L186" rel="#L186">186</span>
<span id="L187" rel="#L187">187</span>
<span id="L188" rel="#L188">188</span>
<span id="L189" rel="#L189">189</span>
<span id="L190" rel="#L190">190</span>
<span id="L191" rel="#L191">191</span>
<span id="L192" rel="#L192">192</span>
<span id="L193" rel="#L193">193</span>
<span id="L194" rel="#L194">194</span>
<span id="L195" rel="#L195">195</span>
<span id="L196" rel="#L196">196</span>
<span id="L197" rel="#L197">197</span>
<span id="L198" rel="#L198">198</span>
<span id="L199" rel="#L199">199</span>
<span id="L200" rel="#L200">200</span>
<span id="L201" rel="#L201">201</span>
<span id="L202" rel="#L202">202</span>
<span id="L203" rel="#L203">203</span>
<span id="L204" rel="#L204">204</span>
<span id="L205" rel="#L205">205</span>
<span id="L206" rel="#L206">206</span>
<span id="L207" rel="#L207">207</span>
<span id="L208" rel="#L208">208</span>
<span id="L209" rel="#L209">209</span>
<span id="L210" rel="#L210">210</span>
<span id="L211" rel="#L211">211</span>
<span id="L212" rel="#L212">212</span>
<span id="L213" rel="#L213">213</span>
<span id="L214" rel="#L214">214</span>
<span id="L215" rel="#L215">215</span>
<span id="L216" rel="#L216">216</span>
<span id="L217" rel="#L217">217</span>
<span id="L218" rel="#L218">218</span>
<span id="L219" rel="#L219">219</span>
<span id="L220" rel="#L220">220</span>
<span id="L221" rel="#L221">221</span>
<span id="L222" rel="#L222">222</span>
<span id="L223" rel="#L223">223</span>
<span id="L224" rel="#L224">224</span>
<span id="L225" rel="#L225">225</span>
<span id="L226" rel="#L226">226</span>
<span id="L227" rel="#L227">227</span>
<span id="L228" rel="#L228">228</span>
<span id="L229" rel="#L229">229</span>
<span id="L230" rel="#L230">230</span>
<span id="L231" rel="#L231">231</span>
<span id="L232" rel="#L232">232</span>
<span id="L233" rel="#L233">233</span>
<span id="L234" rel="#L234">234</span>
<span id="L235" rel="#L235">235</span>
<span id="L236" rel="#L236">236</span>
<span id="L237" rel="#L237">237</span>
<span id="L238" rel="#L238">238</span>
<span id="L239" rel="#L239">239</span>
<span id="L240" rel="#L240">240</span>
<span id="L241" rel="#L241">241</span>
<span id="L242" rel="#L242">242</span>
<span id="L243" rel="#L243">243</span>
<span id="L244" rel="#L244">244</span>
<span id="L245" rel="#L245">245</span>
<span id="L246" rel="#L246">246</span>
<span id="L247" rel="#L247">247</span>
<span id="L248" rel="#L248">248</span>
<span id="L249" rel="#L249">249</span>
<span id="L250" rel="#L250">250</span>
<span id="L251" rel="#L251">251</span>
<span id="L252" rel="#L252">252</span>
<span id="L253" rel="#L253">253</span>
<span id="L254" rel="#L254">254</span>
<span id="L255" rel="#L255">255</span>
<span id="L256" rel="#L256">256</span>
<span id="L257" rel="#L257">257</span>
<span id="L258" rel="#L258">258</span>
<span id="L259" rel="#L259">259</span>
<span id="L260" rel="#L260">260</span>
<span id="L261" rel="#L261">261</span>
<span id="L262" rel="#L262">262</span>
<span id="L263" rel="#L263">263</span>
<span id="L264" rel="#L264">264</span>
<span id="L265" rel="#L265">265</span>
<span id="L266" rel="#L266">266</span>
<span id="L267" rel="#L267">267</span>
<span id="L268" rel="#L268">268</span>
<span id="L269" rel="#L269">269</span>
<span id="L270" rel="#L270">270</span>
<span id="L271" rel="#L271">271</span>
<span id="L272" rel="#L272">272</span>
<span id="L273" rel="#L273">273</span>
<span id="L274" rel="#L274">274</span>
<span id="L275" rel="#L275">275</span>
<span id="L276" rel="#L276">276</span>
<span id="L277" rel="#L277">277</span>
<span id="L278" rel="#L278">278</span>
<span id="L279" rel="#L279">279</span>
<span id="L280" rel="#L280">280</span>
<span id="L281" rel="#L281">281</span>
<span id="L282" rel="#L282">282</span>
<span id="L283" rel="#L283">283</span>
<span id="L284" rel="#L284">284</span>
<span id="L285" rel="#L285">285</span>
<span id="L286" rel="#L286">286</span>
<span id="L287" rel="#L287">287</span>
<span id="L288" rel="#L288">288</span>
<span id="L289" rel="#L289">289</span>
<span id="L290" rel="#L290">290</span>
<span id="L291" rel="#L291">291</span>
<span id="L292" rel="#L292">292</span>
<span id="L293" rel="#L293">293</span>
<span id="L294" rel="#L294">294</span>
<span id="L295" rel="#L295">295</span>
<span id="L296" rel="#L296">296</span>
<span id="L297" rel="#L297">297</span>
<span id="L298" rel="#L298">298</span>
<span id="L299" rel="#L299">299</span>
<span id="L300" rel="#L300">300</span>
<span id="L301" rel="#L301">301</span>
<span id="L302" rel="#L302">302</span>
<span id="L303" rel="#L303">303</span>
<span id="L304" rel="#L304">304</span>
<span id="L305" rel="#L305">305</span>
<span id="L306" rel="#L306">306</span>
<span id="L307" rel="#L307">307</span>
<span id="L308" rel="#L308">308</span>
<span id="L309" rel="#L309">309</span>
<span id="L310" rel="#L310">310</span>
<span id="L311" rel="#L311">311</span>
<span id="L312" rel="#L312">312</span>
<span id="L313" rel="#L313">313</span>
<span id="L314" rel="#L314">314</span>
<span id="L315" rel="#L315">315</span>
<span id="L316" rel="#L316">316</span>
<span id="L317" rel="#L317">317</span>
<span id="L318" rel="#L318">318</span>
<span id="L319" rel="#L319">319</span>
<span id="L320" rel="#L320">320</span>
<span id="L321" rel="#L321">321</span>
<span id="L322" rel="#L322">322</span>
<span id="L323" rel="#L323">323</span>
<span id="L324" rel="#L324">324</span>
<span id="L325" rel="#L325">325</span>
<span id="L326" rel="#L326">326</span>
<span id="L327" rel="#L327">327</span>
<span id="L328" rel="#L328">328</span>
<span id="L329" rel="#L329">329</span>
<span id="L330" rel="#L330">330</span>
<span id="L331" rel="#L331">331</span>
<span id="L332" rel="#L332">332</span>
<span id="L333" rel="#L333">333</span>
<span id="L334" rel="#L334">334</span>
<span id="L335" rel="#L335">335</span>
<span id="L336" rel="#L336">336</span>
<span id="L337" rel="#L337">337</span>
<span id="L338" rel="#L338">338</span>
<span id="L339" rel="#L339">339</span>
<span id="L340" rel="#L340">340</span>
<span id="L341" rel="#L341">341</span>
<span id="L342" rel="#L342">342</span>
<span id="L343" rel="#L343">343</span>
<span id="L344" rel="#L344">344</span>
<span id="L345" rel="#L345">345</span>
<span id="L346" rel="#L346">346</span>
<span id="L347" rel="#L347">347</span>
<span id="L348" rel="#L348">348</span>
<span id="L349" rel="#L349">349</span>
<span id="L350" rel="#L350">350</span>
<span id="L351" rel="#L351">351</span>
<span id="L352" rel="#L352">352</span>
<span id="L353" rel="#L353">353</span>
<span id="L354" rel="#L354">354</span>
<span id="L355" rel="#L355">355</span>
<span id="L356" rel="#L356">356</span>
<span id="L357" rel="#L357">357</span>
<span id="L358" rel="#L358">358</span>
<span id="L359" rel="#L359">359</span>
<span id="L360" rel="#L360">360</span>
<span id="L361" rel="#L361">361</span>
<span id="L362" rel="#L362">362</span>
<span id="L363" rel="#L363">363</span>
<span id="L364" rel="#L364">364</span>
<span id="L365" rel="#L365">365</span>
<span id="L366" rel="#L366">366</span>
<span id="L367" rel="#L367">367</span>
<span id="L368" rel="#L368">368</span>
<span id="L369" rel="#L369">369</span>
<span id="L370" rel="#L370">370</span>
<span id="L371" rel="#L371">371</span>
<span id="L372" rel="#L372">372</span>
<span id="L373" rel="#L373">373</span>
<span id="L374" rel="#L374">374</span>
<span id="L375" rel="#L375">375</span>
<span id="L376" rel="#L376">376</span>
<span id="L377" rel="#L377">377</span>
<span id="L378" rel="#L378">378</span>
<span id="L379" rel="#L379">379</span>
<span id="L380" rel="#L380">380</span>
<span id="L381" rel="#L381">381</span>
<span id="L382" rel="#L382">382</span>
<span id="L383" rel="#L383">383</span>
<span id="L384" rel="#L384">384</span>
<span id="L385" rel="#L385">385</span>
<span id="L386" rel="#L386">386</span>
<span id="L387" rel="#L387">387</span>
<span id="L388" rel="#L388">388</span>
<span id="L389" rel="#L389">389</span>
<span id="L390" rel="#L390">390</span>
<span id="L391" rel="#L391">391</span>
<span id="L392" rel="#L392">392</span>
<span id="L393" rel="#L393">393</span>
<span id="L394" rel="#L394">394</span>
<span id="L395" rel="#L395">395</span>
<span id="L396" rel="#L396">396</span>
<span id="L397" rel="#L397">397</span>
<span id="L398" rel="#L398">398</span>
<span id="L399" rel="#L399">399</span>
<span id="L400" rel="#L400">400</span>
<span id="L401" rel="#L401">401</span>
<span id="L402" rel="#L402">402</span>
<span id="L403" rel="#L403">403</span>
<span id="L404" rel="#L404">404</span>
<span id="L405" rel="#L405">405</span>
<span id="L406" rel="#L406">406</span>
<span id="L407" rel="#L407">407</span>
<span id="L408" rel="#L408">408</span>
<span id="L409" rel="#L409">409</span>
<span id="L410" rel="#L410">410</span>
<span id="L411" rel="#L411">411</span>
<span id="L412" rel="#L412">412</span>
<span id="L413" rel="#L413">413</span>
<span id="L414" rel="#L414">414</span>
<span id="L415" rel="#L415">415</span>
<span id="L416" rel="#L416">416</span>
<span id="L417" rel="#L417">417</span>
<span id="L418" rel="#L418">418</span>
<span id="L419" rel="#L419">419</span>
<span id="L420" rel="#L420">420</span>
<span id="L421" rel="#L421">421</span>
<span id="L422" rel="#L422">422</span>
<span id="L423" rel="#L423">423</span>
<span id="L424" rel="#L424">424</span>
<span id="L425" rel="#L425">425</span>
<span id="L426" rel="#L426">426</span>
<span id="L427" rel="#L427">427</span>
<span id="L428" rel="#L428">428</span>
<span id="L429" rel="#L429">429</span>
</pre>
          </td>
          <td width="100%">
                <div class="highlight"><pre><div class='line' id='LC1'><span class="cp">#include &quot;KSegmentor.h&quot;</span></div><div class='line' id='LC2'><span class="cp">#include &quot;llist.h&quot;</span></div><div class='line' id='LC3'><span class="cp">#include &quot;sfm_local_chanvese_mex.h&quot;</span></div><div class='line' id='LC4'><span class="cp">#include &quot;interactive_kurvolver-edge.h&quot;</span></div><div class='line' id='LC5'><span class="cp">#include &quot;vtkImageData.h&quot;</span></div><div class='line' id='LC6'><span class="cp">#include &quot;KSandbox.h&quot;</span></div><div class='line' id='LC7'><span class="cp">#include &lt;omp.h&gt;</span></div><div class='line' id='LC8'><span class="cp">#include &lt;string&gt;</span></div><div class='line' id='LC9'><span class="cp">#include &lt;sstream&gt;</span></div><div class='line' id='LC10'><span class="cp">#include &lt;opencv2/highgui/highgui.hpp&gt;</span></div><div class='line' id='LC11'><br/></div><div class='line' id='LC12'><span class="k">using</span> <span class="n">std</span><span class="o">::</span><span class="n">string</span><span class="p">;</span></div><div class='line' id='LC13'><span class="k">using</span> <span class="n">cv</span><span class="o">::</span><span class="n">Mat</span><span class="p">;</span></div><div class='line' id='LC14'><br/></div><div class='line' id='LC15'><span class="c1">//these global variables are no good, need to fix later</span></div><div class='line' id='LC16'><span class="k">extern</span> <span class="kt">double</span> <span class="n">ain</span><span class="p">,</span> <span class="n">aout</span><span class="p">,</span> <span class="n">auser</span><span class="p">;</span> <span class="c1">// means</span></div><div class='line' id='LC17'><span class="k">extern</span> <span class="kt">double</span> <span class="o">*</span><span class="n">pdfin</span><span class="p">,</span> <span class="o">*</span><span class="n">pdfout</span><span class="p">,</span> <span class="o">*</span><span class="n">pdfuser</span><span class="p">;</span></div><div class='line' id='LC18'><span class="k">extern</span> <span class="kt">long</span> <span class="n">numdims</span><span class="p">;</span></div><div class='line' id='LC19'><span class="k">extern</span> <span class="kt">double</span> <span class="n">engEval</span><span class="p">;</span></div><div class='line' id='LC20'><span class="k">extern</span> <span class="kt">bool</span> <span class="n">UseInitContour</span><span class="p">;</span></div><div class='line' id='LC21'><span class="k">extern</span> <span class="kt">double</span> <span class="o">*</span><span class="n">Ain</span><span class="p">,</span> <span class="o">*</span><span class="n">Aout</span><span class="p">,</span> <span class="o">*</span><span class="n">Sin</span><span class="p">,</span> <span class="o">*</span><span class="n">Sout</span><span class="p">;</span> <span class="c1">//local means</span></div><div class='line' id='LC22'><br/></div><div class='line' id='LC23'><span class="k">namespace</span> <span class="n">vrcl</span></div><div class='line' id='LC24'><span class="p">{</span></div><div class='line' id='LC25'><br/></div><div class='line' id='LC26'><span class="kt">void</span> <span class="n">test_OpenMP</span><span class="p">()</span></div><div class='line' id='LC27'><span class="p">{</span></div><div class='line' id='LC28'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">nthreads</span><span class="p">,</span> <span class="n">tid</span><span class="p">;</span></div><div class='line' id='LC29'>&nbsp;&nbsp;<span class="n">omp_set_num_threads</span><span class="p">(</span><span class="mi">8</span><span class="p">);</span></div><div class='line' id='LC30'>&nbsp;&nbsp;<span class="n">nthreads</span> <span class="o">=</span> <span class="n">omp_get_num_threads</span><span class="p">();</span></div><div class='line' id='LC31'><span class="cp">  #pragma omp parallel shared(nthreads, tid)</span></div><div class='line' id='LC32'>&nbsp;&nbsp;<span class="p">{</span> <span class="c1">// fork some threads, each one does one expensive operation</span></div><div class='line' id='LC33'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">tid</span> <span class="o">=</span> <span class="n">omp_get_thread_num</span><span class="p">();</span></div><div class='line' id='LC34'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span> <span class="n">tid</span> <span class="o">==</span> <span class="mi">0</span> <span class="p">)</span>      <span class="p">{</span> <span class="p">}</span></div><div class='line' id='LC35'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">else</span> <span class="k">if</span><span class="p">(</span> <span class="n">tid</span> <span class="o">==</span> <span class="mi">1</span> <span class="p">)</span> <span class="p">{</span> <span class="p">}</span></div><div class='line' id='LC36'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">else</span> <span class="k">if</span><span class="p">(</span> <span class="n">tid</span> <span class="o">==</span> <span class="mi">2</span> <span class="p">)</span> <span class="p">{</span> <span class="p">}</span></div><div class='line' id='LC37'><br/></div><div class='line' id='LC38'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC39'><span class="p">}</span></div><div class='line' id='LC40'><br/></div><div class='line' id='LC41'><span class="cm">/** default curvature penalty term. can be set externally when a KSegmentor is made. */</span></div><div class='line' id='LC42'><span class="kt">double</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">defaultKappaParam</span> <span class="o">=</span> <span class="mf">0.2</span><span class="p">;</span></div><div class='line' id='LC43'><br/></div><div class='line' id='LC44'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">saveMatToPNG</span><span class="p">(</span> <span class="k">const</span> <span class="n">cv</span><span class="o">::</span><span class="n">Mat</span><span class="o">&amp;</span> <span class="n">sliceImg</span><span class="p">,</span> <span class="k">const</span> <span class="n">std</span><span class="o">::</span><span class="n">string</span><span class="o">&amp;</span> <span class="n">fileName</span> <span class="p">)</span></div><div class='line' id='LC45'><span class="p">{</span></div><div class='line' id='LC46'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">std</span><span class="o">::</span><span class="n">stringstream</span>  <span class="n">ss</span><span class="p">;</span></div><div class='line' id='LC47'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ss</span> <span class="o">&lt;&lt;</span> <span class="n">fileName</span><span class="p">;</span></div><div class='line' id='LC48'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">bool</span> <span class="n">name_is_png</span><span class="o">=</span><span class="p">(</span><span class="mi">0</span><span class="o">==</span><span class="n">std</span><span class="o">::</span><span class="n">string</span><span class="p">(</span><span class="s">&quot;.png&quot;</span><span class="p">).</span><span class="n">compare</span><span class="p">(</span><span class="n">fileName</span><span class="p">.</span><span class="n">substr</span><span class="p">(</span><span class="n">fileName</span><span class="p">.</span><span class="n">size</span><span class="p">()</span><span class="o">-</span><span class="mi">4</span><span class="p">,</span><span class="mi">4</span><span class="p">))</span> <span class="p">);</span></div><div class='line' id='LC49'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span> <span class="o">!</span><span class="n">name_is_png</span> <span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC50'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ss</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;.png&quot;</span> <span class="p">;</span></div><div class='line' id='LC51'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC52'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">string</span> <span class="n">png_name</span> <span class="o">=</span> <span class="n">ss</span><span class="p">.</span><span class="n">str</span><span class="p">();</span></div><div class='line' id='LC53'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">&quot; reference png file: &quot;</span> <span class="o">&lt;&lt;</span> <span class="n">png_name</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span></div><div class='line' id='LC54'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cv</span><span class="o">::</span><span class="n">Mat</span> <span class="n">source</span><span class="p">;</span></div><div class='line' id='LC55'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cv</span><span class="o">::</span><span class="n">flip</span><span class="p">(</span> <span class="o">-</span><span class="mf">1.0</span> <span class="o">*</span> <span class="n">sliceImg</span><span class="p">,</span> <span class="n">source</span><span class="p">,</span> <span class="mi">0</span> <span class="cm">/* flipVert */</span> <span class="p">);</span></div><div class='line' id='LC56'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">double</span> <span class="n">dmin</span><span class="p">,</span> <span class="n">dmax</span><span class="p">;</span></div><div class='line' id='LC57'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cv</span><span class="o">::</span><span class="n">minMaxLoc</span><span class="p">(</span> <span class="n">source</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">dmin</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">dmax</span> <span class="p">);</span></div><div class='line' id='LC58'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cv</span><span class="o">::</span><span class="n">Mat</span> <span class="n">saveImg</span> <span class="o">=</span> <span class="p">(</span><span class="mf">255.0</span> <span class="o">/</span> <span class="p">(</span><span class="n">dmax</span> <span class="o">-</span> <span class="n">dmin</span> <span class="p">))</span> <span class="o">*</span> <span class="p">(</span><span class="n">source</span> <span class="o">-</span> <span class="n">dmin</span><span class="p">);</span></div><div class='line' id='LC59'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cv</span><span class="o">::</span><span class="n">imwrite</span><span class="p">(</span><span class="n">png_name</span><span class="p">,</span> <span class="n">saveImg</span> <span class="p">);</span></div><div class='line' id='LC60'><span class="p">}</span></div><div class='line' id='LC61'><br/></div><div class='line' id='LC62'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">saveCurrentSliceToPNG</span><span class="p">(</span><span class="k">const</span> <span class="n">std</span><span class="o">::</span><span class="n">string</span> <span class="o">&amp;</span><span class="n">fileName</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC63'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;saving png file, currSlice = &quot;</span> <span class="o">&lt;&lt;</span> <span class="n">currSlice</span> <span class="o">&lt;&lt;</span> <span class="s">&quot; ... &quot;</span><span class="p">;</span></div><div class='line' id='LC64'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">saveMatToPNG</span><span class="p">(</span><span class="n">U_integral</span><span class="p">[</span><span class="n">currSlice</span><span class="p">],</span> <span class="n">fileName</span> <span class="p">);</span></div><div class='line' id='LC65'><span class="p">}</span></div><div class='line' id='LC66'><br/></div><div class='line' id='LC67'><br/></div><div class='line' id='LC68'><span class="n">KSegmentor</span><span class="o">::</span><span class="n">KSegmentor</span><span class="p">(</span><span class="n">vtkImageData</span> <span class="o">*</span><span class="n">image</span><span class="p">,</span> <span class="n">vtkImageData</span> <span class="o">*</span><span class="n">label</span><span class="p">,</span> <span class="kt">int</span> <span class="n">sliceIndex</span><span class="p">,</span> <span class="kt">bool</span> <span class="n">contInit</span><span class="p">)</span></div><div class='line' id='LC69'><span class="p">{</span></div><div class='line' id='LC70'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">int</span> <span class="n">num_slices</span> <span class="o">=</span> <span class="n">image</span><span class="o">-&gt;</span><span class="n">GetDimensions</span><span class="p">()[</span><span class="mi">2</span><span class="p">];</span></div><div class='line' id='LC71'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">imageVol</span><span class="o">=</span><span class="n">image</span><span class="p">;</span></div><div class='line' id='LC72'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">labelVol</span><span class="o">=</span><span class="n">label</span><span class="p">;</span></div><div class='line' id='LC73'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">currSlice</span><span class="o">=</span><span class="n">sliceIndex</span><span class="p">;</span></div><div class='line' id='LC74'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">mdims</span> <span class="o">=</span> <span class="k">new</span> <span class="kt">int</span><span class="p">[</span><span class="mi">3</span><span class="p">];</span></div><div class='line' id='LC75'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">image</span><span class="o">-&gt;</span><span class="n">GetDimensions</span><span class="p">(</span> <span class="n">mdims</span> <span class="p">);</span></div><div class='line' id='LC76'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">numberdims</span><span class="o">=</span><span class="mi">2</span><span class="p">;</span></div><div class='line' id='LC77'><br/></div><div class='line' id='LC78'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">mdims</span><span class="p">[</span><span class="mi">2</span><span class="p">]</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span></div><div class='line' id='LC79'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">penaltyAlpha</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span></div><div class='line' id='LC80'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">seed</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span></div><div class='line' id='LC81'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">useContInit</span><span class="o">=</span><span class="n">contInit</span><span class="p">;</span></div><div class='line' id='LC82'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">display</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span></div><div class='line' id='LC83'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">dthresh</span><span class="o">=</span><span class="mi">500</span><span class="p">;</span></div><div class='line' id='LC84'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">iter</span><span class="o">=</span><span class="mi">200</span><span class="p">;</span></div><div class='line' id='LC85'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">lambda</span><span class="o">=</span><span class="n">defaultKappaParam</span><span class="p">;</span> <span class="c1">// this could/should be user togglable!</span></div><div class='line' id='LC86'><br/></div><div class='line' id='LC87'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lz</span><span class="o">=</span><span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC88'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Ln1</span><span class="o">=</span> <span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC89'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Ln2</span><span class="o">=</span> <span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC90'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lp1</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC91'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lp2</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC92'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Sz</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC93'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Sn1</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC94'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Sn2</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC95'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Sp1</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC96'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Sp2</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC97'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lin2out</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC98'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lout2in</span><span class="o">=</span><span class="nb">NULL</span> <span class="p">;</span></div><div class='line' id='LC99'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">seg</span><span class="o">=</span><span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC100'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">img</span><span class="o">=</span><span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC101'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">mask</span><span class="o">=</span><span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC102'><br/></div><div class='line' id='LC103'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">double</span> <span class="n">spacing_mm</span><span class="p">[</span><span class="mi">3</span><span class="p">];</span></div><div class='line' id='LC104'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">image</span><span class="o">-&gt;</span><span class="n">GetSpacing</span><span class="p">(</span> <span class="n">spacing_mm</span> <span class="p">);</span></div><div class='line' id='LC105'><br/></div><div class='line' id='LC106'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">// want rad to be &#39;10&#39; for 512 . A 512x512 mri with xy spacing 0.3mm is 153.6000 across</span></div><div class='line' id='LC107'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">// &quot;10&quot; pixels is 3mm in this context.</span></div><div class='line' id='LC108'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">rad</span> <span class="o">=</span> <span class="mf">3.0</span> <span class="o">/</span> <span class="n">std</span><span class="o">::</span><span class="n">max</span><span class="p">(</span> <span class="n">spacing_mm</span><span class="p">[</span><span class="mi">0</span><span class="p">],</span><span class="n">spacing_mm</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="p">);</span> <span class="c1">// about 3mm in physical units</span></div><div class='line' id='LC109'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">rad</span> <span class="o">=</span> <span class="n">std</span><span class="o">::</span><span class="n">min</span><span class="p">(</span><span class="mf">18.0</span><span class="p">,</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">rad</span><span class="p">);</span> <span class="c1">// force non-huge radius if the spacing is retarded</span></div><div class='line' id='LC110'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">rad</span> <span class="o">=</span> <span class="n">std</span><span class="o">::</span><span class="n">max</span><span class="p">(</span><span class="mf">3.0</span><span class="p">,</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">rad</span><span class="p">);</span> <span class="c1">// force non-tiny radius if the spacing is retarded</span></div><div class='line' id='LC111'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;segmentor using ROI size: &quot;</span> <span class="o">&lt;&lt;</span> <span class="n">rad</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span></div><div class='line' id='LC112'><br/></div><div class='line' id='LC113'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">img</span>        <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="p">];</span></div><div class='line' id='LC114'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">mask</span>       <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="p">];</span></div><div class='line' id='LC115'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_I_slice</span>  <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="p">];</span></div><div class='line' id='LC116'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_slice</span>  <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="p">];</span></div><div class='line' id='LC117'><br/></div><div class='line' id='LC118'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span> <span class="o">=</span> <span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkImageData</span><span class="o">&gt;::</span><span class="n">New</span><span class="p">();</span></div><div class='line' id='LC119'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_image</span><span class="o">=</span> <span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkImageData</span><span class="o">&gt;::</span><span class="n">New</span><span class="p">();</span></div><div class='line' id='LC120'><br/></div><div class='line' id='LC121'><br/></div><div class='line' id='LC122'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="o">-&gt;</span><span class="n">SetExtent</span><span class="p">(</span><span class="n">image</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">());</span></div><div class='line' id='LC123'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="o">-&gt;</span><span class="n">SetScalarTypeToDouble</span><span class="p">();</span></div><div class='line' id='LC124'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrIntegral_Image</span> <span class="o">=</span> <span class="k">static_cast</span><span class="o">&lt;</span><span class="kt">double</span><span class="o">*&gt;</span><span class="p">(</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="o">-&gt;</span><span class="n">GetScalarPointer</span><span class="p">());</span></div><div class='line' id='LC125'><br/></div><div class='line' id='LC126'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">SetExtent</span><span class="p">(</span><span class="n">image</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">());</span></div><div class='line' id='LC127'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">SetScalarTypeToDouble</span><span class="p">();</span></div><div class='line' id='LC128'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrU_t_Image</span> <span class="o">=</span> <span class="k">static_cast</span><span class="o">&lt;</span><span class="kt">double</span><span class="o">*&gt;</span><span class="p">(</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">GetScalarPointer</span><span class="p">());</span></div><div class='line' id='LC129'><br/></div><div class='line' id='LC130'><br/></div><div class='line' id='LC131'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span> <span class="o">=</span> <span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkImageReslice</span><span class="o">&gt;::</span><span class="n">New</span><span class="p">();</span></div><div class='line' id='LC132'><br/></div><div class='line' id='LC133'><br/></div><div class='line' id='LC134'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">imgRange</span>   <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span><span class="mi">2</span><span class="p">];</span></div><div class='line' id='LC135'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">labelRange</span> <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span><span class="mi">2</span><span class="p">];</span></div><div class='line' id='LC136'><br/></div><div class='line' id='LC137'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">phi</span>        <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">2</span><span class="p">]];</span></div><div class='line' id='LC138'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">label</span>      <span class="o">=</span> <span class="k">new</span> <span class="kt">double</span><span class="p">[</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">2</span><span class="p">]];</span></div><div class='line' id='LC139'><br/></div><div class='line' id='LC140'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">U_integral</span> <span class="o">=</span> <span class="n">std</span><span class="o">::</span><span class="n">vector</span><span class="o">&lt;</span> <span class="n">cv</span><span class="o">::</span><span class="n">Mat</span> <span class="o">&gt;</span><span class="p">(</span> <span class="n">num_slices</span> <span class="p">);</span></div><div class='line' id='LC141'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">U_t</span>        <span class="o">=</span> <span class="n">std</span><span class="o">::</span><span class="n">vector</span><span class="o">&lt;</span> <span class="n">cv</span><span class="o">::</span><span class="n">Mat</span> <span class="o">&gt;</span><span class="p">(</span> <span class="n">num_slices</span> <span class="p">);</span></div><div class='line' id='LC142'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;I think the # of rows is: &quot;</span> <span class="o">&lt;&lt;</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;, # of cols is: &quot;</span> <span class="o">&lt;&lt;</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span></div><div class='line' id='LC143'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span><span class="p">(</span> <span class="kt">int</span> <span class="n">k</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">k</span> <span class="o">&lt;</span> <span class="n">num_slices</span><span class="p">;</span> <span class="n">k</span><span class="o">++</span> <span class="p">)</span></div><div class='line' id='LC144'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">{</span> <span class="cm">/** per-slice user updates  */</span></div><div class='line' id='LC145'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">U_integral</span><span class="p">[</span><span class="n">k</span><span class="p">]</span> <span class="o">=</span> <span class="n">cv</span><span class="o">::</span><span class="n">Mat</span><span class="o">::</span><span class="n">zeros</span><span class="p">(</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">],</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">],</span> <span class="n">CV_64F</span> <span class="p">);</span></div><div class='line' id='LC146'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">U_t</span><span class="p">[</span><span class="n">k</span><span class="p">]</span>        <span class="o">=</span> <span class="n">cv</span><span class="o">::</span><span class="n">Mat</span><span class="o">::</span><span class="n">zeros</span><span class="p">(</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">],</span> <span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">],</span> <span class="n">CV_64F</span> <span class="p">);</span></div><div class='line' id='LC147'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC148'><br/></div><div class='line' id='LC149'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">iList</span><span class="o">=</span><span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC150'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">jList</span><span class="o">=</span><span class="nb">NULL</span><span class="p">;</span></div><div class='line' id='LC151'><br/></div><div class='line' id='LC152'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">numdims</span><span class="o">=</span><span class="n">numberdims</span><span class="p">;</span></div><div class='line' id='LC153'><br/></div><div class='line' id='LC154'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">useContInit</span><span class="p">)</span></div><div class='line' id='LC155'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">{</span></div><div class='line' id='LC156'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">initializeUserInputImageWithContour</span><span class="p">();</span></div><div class='line' id='LC157'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">std</span><span class="o">::</span><span class="n">cout</span><span class="o">&lt;&lt;</span><span class="s">&quot;Initializing user input using label data&quot;</span><span class="o">&lt;&lt;</span><span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span></div><div class='line' id='LC158'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC159'><br/></div><div class='line' id='LC160'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dims</span><span class="p">[</span><span class="mi">2</span><span class="p">]</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span></div><div class='line' id='LC161'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span></div><div class='line' id='LC162'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">switch</span><span class="p">(</span><span class="n">numdims</span><span class="p">){</span></div><div class='line' id='LC163'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">case</span> <span class="mi">3</span><span class="o">:</span></div><div class='line' id='LC164'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dimz</span> <span class="o">=</span> <span class="p">(</span><span class="kt">int</span><span class="p">)</span><span class="n">mdims</span><span class="p">[</span><span class="mi">2</span><span class="p">];</span></div><div class='line' id='LC165'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dims</span><span class="p">[</span><span class="mi">2</span><span class="p">]</span> <span class="o">=</span> <span class="n">dimz</span><span class="p">;</span></div><div class='line' id='LC166'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">case</span> <span class="mi">2</span><span class="o">:</span></div><div class='line' id='LC167'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dimx</span> <span class="o">=</span> <span class="p">(</span><span class="kt">int</span><span class="p">)</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">];</span></div><div class='line' id='LC168'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="o">=</span> <span class="n">dimx</span><span class="p">;</span></div><div class='line' id='LC169'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">case</span> <span class="mi">1</span><span class="o">:</span></div><div class='line' id='LC170'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dimy</span> <span class="o">=</span> <span class="p">(</span><span class="kt">int</span><span class="p">)</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">];</span></div><div class='line' id='LC171'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span> <span class="o">=</span> <span class="n">dimy</span><span class="p">;</span></div><div class='line' id='LC172'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC173'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dims</span><span class="p">[</span><span class="mi">3</span><span class="p">]</span> <span class="o">=</span> <span class="n">dims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">dims</span><span class="p">[</span><span class="mi">1</span><span class="p">];</span></div><div class='line' id='LC174'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">dims</span><span class="p">[</span><span class="mi">4</span><span class="p">]</span> <span class="o">=</span> <span class="n">dims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">dims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">*</span><span class="n">dims</span><span class="p">[</span><span class="mi">2</span><span class="p">];</span></div><div class='line' id='LC175'><br/></div><div class='line' id='LC176'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">seg</span> <span class="o">=</span> <span class="k">new</span>  <span class="kt">unsigned</span> <span class="kt">short</span><span class="p">[</span><span class="n">dims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">dims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">*</span><span class="n">dims</span><span class="p">[</span><span class="mi">2</span><span class="p">]];</span></div><div class='line' id='LC177'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">initializeData</span><span class="p">();</span></div><div class='line' id='LC178'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">intializeLevelSet</span><span class="p">();</span></div><div class='line' id='LC179'><span class="p">}</span></div><div class='line' id='LC180'><br/></div><div class='line' id='LC181'><br/></div><div class='line' id='LC182'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">initializeUserInputImageWithContour</span><span class="p">(){</span></div><div class='line' id='LC183'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">int</span> <span class="n">element</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span></div><div class='line' id='LC184'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">unsigned</span> <span class="kt">short</span><span class="o">*</span> <span class="n">label3DPointer</span> <span class="o">=</span> <span class="k">static_cast</span><span class="o">&lt;</span><span class="kt">unsigned</span> <span class="kt">short</span><span class="o">*&gt;</span><span class="p">(</span><span class="n">labelVol</span><span class="o">-&gt;</span><span class="n">GetScalarPointer</span><span class="p">());</span></div><div class='line' id='LC185'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">i</span><span class="o">&lt;=</span><span class="n">imageVol</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">1</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span>  <span class="p">{</span></div><div class='line' id='LC186'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">j</span><span class="o">&lt;=</span><span class="n">imageVol</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">3</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC187'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span><span class="p">(</span><span class="kt">int</span> <span class="n">k</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span><span class="n">k</span><span class="o">&lt;=</span><span class="n">imageVol</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">5</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span><span class="n">k</span><span class="o">++</span><span class="p">){</span></div><div class='line' id='LC188'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//this-&gt;accumulateAndIntegrateUserInputInUserImages( this-&gt;labelVol-&gt;GetScalarComponentAsDouble(i,j,k,0),i,j,k);</span></div><div class='line' id='LC189'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">accumulateUserInputInUserInputImages</span><span class="p">((</span><span class="kt">double</span><span class="p">)</span><span class="n">label3DPointer</span><span class="p">[</span><span class="n">element</span><span class="p">],</span><span class="n">element</span><span class="p">);</span></div><div class='line' id='LC190'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">element</span><span class="o">++</span><span class="p">;</span></div><div class='line' id='LC191'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC192'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC193'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC194'>&nbsp;&nbsp;<span class="k">for</span><span class="p">(</span><span class="kt">int</span> <span class="n">k</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span><span class="n">k</span><span class="o">&lt;=</span><span class="n">imageVol</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">5</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span><span class="n">k</span><span class="o">++</span><span class="p">)</span></div><div class='line' id='LC195'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">integrateUserInputInUserInputImage</span><span class="p">(</span><span class="n">k</span><span class="p">);</span></div><div class='line' id='LC196'><span class="p">}</span></div><div class='line' id='LC197'><br/></div><div class='line' id='LC198'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">accumulateUserInput</span><span class="p">(</span> <span class="kt">double</span> <span class="n">value</span><span class="p">,</span> <span class="kt">int</span> <span class="n">j</span><span class="p">,</span> <span class="kt">int</span> <span class="n">i</span><span class="p">,</span> <span class="kt">int</span> <span class="n">k</span> <span class="p">)</span></div><div class='line' id='LC199'><span class="p">{</span> <span class="c1">// clicking: set the U_t values, +/- 1.0 ( phi &lt; 0 is inside! )</span></div><div class='line' id='LC200'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">assert</span><span class="p">(</span> <span class="n">j</span> <span class="o">&lt;</span> <span class="n">U_t</span><span class="p">[</span><span class="n">k</span><span class="p">].</span><span class="n">cols</span> <span class="p">);</span></div><div class='line' id='LC201'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">assert</span><span class="p">(</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">U_t</span><span class="p">[</span><span class="n">k</span><span class="p">].</span><span class="n">rows</span> <span class="p">);</span></div><div class='line' id='LC202'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">double</span> <span class="n">user_input</span>      <span class="o">=</span> <span class="o">-</span><span class="mf">1.0</span> <span class="o">*</span> <span class="p">(</span> <span class="n">value</span> <span class="o">&gt;</span> <span class="mf">0.5</span> <span class="p">)</span> <span class="o">+</span> <span class="mf">1.0</span> <span class="o">*</span> <span class="p">(</span> <span class="n">value</span> <span class="o">&lt;=</span> <span class="mf">0.5</span> <span class="p">);</span></div><div class='line' id='LC203'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">U_t</span><span class="p">[</span><span class="n">k</span><span class="p">].</span><span class="n">at</span><span class="o">&lt;</span><span class="kt">double</span><span class="o">&gt;</span><span class="p">(</span><span class="n">i</span><span class="p">,</span><span class="n">j</span><span class="p">)</span> <span class="o">=</span> <span class="n">user_input</span> <span class="p">;</span></div><div class='line' id='LC204'><br/></div><div class='line' id='LC205'><span class="p">}</span></div><div class='line' id='LC206'><br/></div><div class='line' id='LC207'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">accumulateUserInputInUserInputImages</span><span class="p">(</span> <span class="kt">double</span> <span class="n">value</span><span class="p">,</span><span class="k">const</span> <span class="kt">unsigned</span> <span class="kt">int</span> <span class="n">element</span><span class="p">)</span></div><div class='line' id='LC208'><span class="p">{</span></div><div class='line' id='LC209'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">double</span> <span class="n">user_input</span>      <span class="o">=</span> <span class="o">-</span><span class="mf">1.0</span> <span class="o">*</span> <span class="p">(</span> <span class="n">value</span> <span class="o">&gt;</span> <span class="mf">0.5</span> <span class="p">)</span> <span class="o">+</span> <span class="mf">1.0</span> <span class="o">*</span> <span class="p">(</span> <span class="n">value</span> <span class="o">&lt;=</span> <span class="mf">0.5</span> <span class="p">);</span></div><div class='line' id='LC210'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrU_t_Image</span><span class="p">[</span><span class="n">element</span><span class="p">]</span><span class="o">=</span><span class="n">user_input</span><span class="p">;</span></div><div class='line' id='LC211'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//Integrate user input</span></div><div class='line' id='LC212'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//this-&gt;U_t_image-&gt;SetScalarComponentFromDouble(i,j,k,0,0.5*this-&gt;U_Integral_image-&gt;GetScalarComponentAsDouble(i,j,k,0));</span></div><div class='line' id='LC213'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//this-&gt;ptrU_t_Image[element]=this-&gt;ptrIntegral_Image[element]*0.5;</span></div><div class='line' id='LC214'><span class="p">}</span></div><div class='line' id='LC215'><br/></div><div class='line' id='LC216'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">integrateUserInput</span><span class="p">(</span> <span class="kt">int</span> <span class="n">k</span> <span class="p">)</span></div><div class='line' id='LC217'><span class="p">{</span></div><div class='line' id='LC218'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">U_integral</span><span class="p">[</span><span class="n">k</span><span class="p">]</span>  <span class="o">+=</span> <span class="n">U_t</span><span class="p">[</span><span class="n">k</span><span class="p">];</span></div><div class='line' id='LC219'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">U_t</span><span class="p">[</span><span class="n">k</span><span class="p">]</span>          <span class="o">=</span> <span class="mf">0.5</span> <span class="o">*</span> <span class="n">U_t</span><span class="p">[</span><span class="n">k</span><span class="p">];</span></div><div class='line' id='LC220'><span class="p">}</span></div><div class='line' id='LC221'><br/></div><div class='line' id='LC222'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">integrateUserInputInUserInputImage</span><span class="p">(</span> <span class="kt">int</span> <span class="n">k</span> <span class="p">)</span></div><div class='line' id='LC223'><span class="p">{</span></div><div class='line' id='LC224'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">unsigned</span> <span class="kt">int</span> <span class="n">element3D</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span></div><div class='line' id='LC225'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">j</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span>  <span class="p">{</span></div><div class='line' id='LC226'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">i</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC227'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">element3D</span><span class="o">=</span><span class="n">k</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span> <span class="o">+</span><span class="n">j</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">+</span><span class="n">i</span><span class="p">;</span></div><div class='line' id='LC228'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrIntegral_Image</span><span class="p">[</span><span class="n">element3D</span><span class="p">]</span><span class="o">+=</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrU_t_Image</span><span class="p">[</span><span class="n">element3D</span><span class="p">];</span></div><div class='line' id='LC229'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrU_t_Image</span><span class="p">[</span><span class="n">element3D</span><span class="p">]</span><span class="o">=</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrU_t_Image</span><span class="p">[</span><span class="n">element3D</span><span class="p">]</span><span class="o">*</span><span class="mf">0.5</span><span class="p">;</span></div><div class='line' id='LC230'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC231'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC232'><br/></div><div class='line' id='LC233'><span class="p">}</span></div><div class='line' id='LC234'><br/></div><div class='line' id='LC235'><br/></div><div class='line' id='LC236'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">setNumIterations</span><span class="p">(</span><span class="kt">int</span> <span class="n">itersToRun</span><span class="p">){</span></div><div class='line' id='LC237'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">iter</span><span class="o">=</span><span class="n">itersToRun</span><span class="p">;</span></div><div class='line' id='LC238'><span class="p">}</span></div><div class='line' id='LC239'><br/></div><div class='line' id='LC240'><br/></div><div class='line' id='LC241'><br/></div><div class='line' id='LC242'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">TransformUserInputImages</span><span class="p">(</span><span class="n">vtkTransform</span><span class="o">*</span> <span class="n">transform</span><span class="p">,</span> <span class="kt">bool</span> <span class="n">invert</span><span class="p">)</span></div><div class='line' id='LC243'><span class="p">{</span></div><div class='line' id='LC244'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">SetInput</span><span class="p">(</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="p">);</span></div><div class='line' id='LC245'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">SetResliceAxesDirectionCosines</span><span class="p">(</span><span class="mi">1</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span>    <span class="mi">0</span><span class="p">,</span><span class="mi">1</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span>     <span class="mi">0</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span><span class="mi">1</span><span class="p">);</span></div><div class='line' id='LC246'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">SetOutputDimensionality</span><span class="p">(</span><span class="mi">3</span><span class="p">);</span></div><div class='line' id='LC247'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span> <span class="p">(</span><span class="n">invert</span><span class="p">)</span></div><div class='line' id='LC248'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">SetResliceTransform</span><span class="p">(</span><span class="n">transform</span><span class="o">-&gt;</span><span class="n">GetInverse</span><span class="p">());</span></div><div class='line' id='LC249'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">else</span></div><div class='line' id='LC250'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">SetResliceTransform</span><span class="p">(</span><span class="n">transform</span><span class="p">);</span></div><div class='line' id='LC251'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">UpdateWholeExtent</span><span class="p">();</span></div><div class='line' id='LC252'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="o">-&gt;</span><span class="n">DeepCopy</span><span class="p">(</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">GetOutput</span><span class="p">());</span></div><div class='line' id='LC253'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrIntegral_Image</span> <span class="o">=</span> <span class="k">static_cast</span><span class="o">&lt;</span><span class="kt">double</span><span class="o">*&gt;</span><span class="p">(</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="o">-&gt;</span><span class="n">GetScalarPointer</span><span class="p">());</span></div><div class='line' id='LC254'><br/></div><div class='line' id='LC255'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">SetInput</span><span class="p">(</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_image</span><span class="p">);</span></div><div class='line' id='LC256'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">SetResliceAxesDirectionCosines</span><span class="p">(</span><span class="mi">1</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span>    <span class="mi">0</span><span class="p">,</span><span class="mi">1</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span>     <span class="mi">0</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span><span class="mi">1</span><span class="p">);</span></div><div class='line' id='LC257'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">Modified</span><span class="p">();</span></div><div class='line' id='LC258'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">UpdateWholeExtent</span><span class="p">();</span></div><div class='line' id='LC259'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">DeepCopy</span><span class="p">(</span><span class="n">m_Reslicer</span><span class="o">-&gt;</span><span class="n">GetOutput</span><span class="p">());</span></div><div class='line' id='LC260'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrU_t_Image</span> <span class="o">=</span> <span class="k">static_cast</span><span class="o">&lt;</span><span class="kt">double</span><span class="o">*&gt;</span><span class="p">(</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">GetScalarPointer</span><span class="p">());</span></div><div class='line' id='LC261'><br/></div><div class='line' id='LC262'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">=</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">1</span><span class="p">]</span><span class="o">-</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">0</span><span class="p">]</span><span class="o">+</span><span class="mi">1</span><span class="p">;</span></div><div class='line' id='LC263'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">=</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">3</span><span class="p">]</span><span class="o">-</span><span class="n">U_t_image</span><span class="o">-&gt;</span><span class="n">GetExtent</span><span class="p">()[</span><span class="mi">2</span><span class="p">]</span><span class="o">+</span><span class="mi">1</span><span class="p">;</span></div><div class='line' id='LC264'><span class="p">}</span></div><div class='line' id='LC265'><br/></div><div class='line' id='LC266'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">initializeData</span><span class="p">()</span></div><div class='line' id='LC267'><span class="p">{</span></div><div class='line' id='LC268'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrCurrImage</span> <span class="o">=</span> <span class="k">static_cast</span><span class="o">&lt;</span><span class="kt">unsigned</span> <span class="kt">short</span><span class="o">*&gt;</span><span class="p">(</span></div><div class='line' id='LC269'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">imageVol</span><span class="o">-&gt;</span><span class="n">GetScalarPointer</span><span class="p">())</span><span class="o">+</span><span class="p">(</span><span class="kt">unsigned</span> <span class="kt">short</span><span class="p">)</span><span class="n">currSlice</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">];</span></div><div class='line' id='LC270'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrCurrLabel</span> <span class="o">=</span> <span class="k">static_cast</span><span class="o">&lt;</span><span class="kt">unsigned</span> <span class="kt">short</span><span class="o">*&gt;</span><span class="p">(</span></div><div class='line' id='LC271'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">labelVol</span><span class="o">-&gt;</span><span class="n">GetScalarPointer</span><span class="p">())</span><span class="o">+</span><span class="p">(</span><span class="kt">unsigned</span> <span class="kt">short</span><span class="p">)</span><span class="n">currSlice</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">];</span></div><div class='line' id='LC272'><br/></div><div class='line' id='LC273'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">imageVol</span><span class="o">-&gt;</span><span class="n">GetScalarRange</span><span class="p">(</span> <span class="n">imgRange</span> <span class="p">);</span></div><div class='line' id='LC274'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">labelVol</span><span class="o">-&gt;</span><span class="n">GetScalarRange</span><span class="p">(</span> <span class="n">labelRange</span> <span class="p">);</span></div><div class='line' id='LC275'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span> <span class="n">abs</span><span class="p">(</span><span class="n">labelRange</span><span class="p">[</span><span class="mi">1</span><span class="p">])</span> <span class="o">&lt;</span> <span class="mf">1e-3</span> <span class="p">)</span></div><div class='line' id='LC276'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">{</span> <span class="c1">// empty label; so set the proper range</span></div><div class='line' id='LC277'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">labelRange</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="o">=</span> <span class="n">KViewerOptions</span><span class="o">::</span><span class="n">getDefaultDrawLabelMaxVal</span><span class="p">();</span></div><div class='line' id='LC278'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC279'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;KSegmentor reads label range: &quot;</span> <span class="o">&lt;&lt;</span> <span class="n">labelRange</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;,&quot;</span> <span class="o">&lt;&lt;</span> <span class="n">labelRange</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span></div><div class='line' id='LC280'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;KSegmentor reads image range: &quot;</span> <span class="o">&lt;&lt;</span> <span class="n">imgRange</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span> <span class="o">&lt;&lt;</span> <span class="s">&quot;,&quot;</span> <span class="o">&lt;&lt;</span> <span class="n">imgRange</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span></div><div class='line' id='LC281'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">assert</span><span class="p">(</span> <span class="mi">0</span> <span class="o">!=</span> <span class="n">imgRange</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="p">);</span> <span class="c1">// what the, all black ?? impossible !</span></div><div class='line' id='LC282'><br/></div><div class='line' id='LC283'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">imgRange</span><span class="o">=</span><span class="n">imgRange</span><span class="p">;</span></div><div class='line' id='LC284'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">integrateUserInput</span><span class="p">(</span> <span class="n">currSlice</span> <span class="p">);</span></div><div class='line' id='LC285'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">integrateUserInputInUserInputImage</span><span class="p">(</span><span class="n">currSlice</span><span class="p">);</span></div><div class='line' id='LC286'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">unsigned</span> <span class="kt">int</span> <span class="n">element3D</span><span class="p">;</span></div><div class='line' id='LC287'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">long</span> <span class="n">elemNum</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span></div><div class='line' id='LC288'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">j</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span>  <span class="p">{</span></div><div class='line' id='LC289'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">i</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC290'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">// indexing definition:  ptr[k*mdims[1]*mdims[0] +j*mdims[0]+i];</span></div><div class='line' id='LC291'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">img</span><span class="p">[</span><span class="n">elemNum</span><span class="p">]</span>        <span class="o">=</span> <span class="p">(</span><span class="kt">double</span><span class="p">)</span> <span class="n">ptrCurrImage</span><span class="p">[</span><span class="n">elemNum</span><span class="p">];</span></div><div class='line' id='LC292'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">mask</span><span class="p">[</span><span class="n">elemNum</span><span class="p">]</span>       <span class="o">=</span> <span class="p">(</span><span class="kt">double</span><span class="p">)</span> <span class="p">(</span> <span class="mi">0</span> <span class="o">&lt;</span> <span class="n">ptrCurrLabel</span><span class="p">[</span><span class="n">elemNum</span><span class="p">]</span> <span class="p">);</span></div><div class='line' id='LC293'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//this-&gt;U_I_slice[elemNum]  = (U_integral[currSlice].ptr&lt;double&gt;(0))[elemNum];</span></div><div class='line' id='LC294'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//New</span></div><div class='line' id='LC295'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">element3D</span><span class="o">=</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">currSlice</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span> <span class="o">+</span><span class="n">j</span><span class="o">*</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">+</span><span class="n">i</span><span class="p">;</span></div><div class='line' id='LC296'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_I_slice</span><span class="p">[</span><span class="n">elemNum</span><span class="p">]</span> <span class="o">=</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">ptrIntegral_Image</span><span class="p">[</span><span class="n">element3D</span><span class="p">];</span></div><div class='line' id='LC297'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">elemNum</span><span class="o">++</span><span class="p">;</span></div><div class='line' id='LC298'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC299'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC300'><br/></div><div class='line' id='LC301'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">bool</span> <span class="n">writeInfoPng</span> <span class="o">=</span> <span class="kc">false</span><span class="p">;</span></div><div class='line' id='LC302'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span> <span class="n">writeInfoPng</span> <span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC303'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="cm">/*std::stringstream ss;</span></div><div class='line' id='LC304'><span class="cm">      ss &lt;&lt; &quot;U_integral_ &quot; &lt;&lt; std::setw(3) &lt;&lt; std::setfill(&#39;0&#39;) &lt;&lt; currSlice &lt;&lt; &quot;.png&quot;;</span></div><div class='line' id='LC305'><span class="cm">      saveCurrentSliceToPNG( ss.str() );*/</span></div><div class='line' id='LC306'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkMetaImageWriter</span><span class="o">&gt;</span> <span class="n">labelWriter</span><span class="o">=</span>   <span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkMetaImageWriter</span><span class="o">&gt;::</span><span class="n">New</span><span class="p">();</span></div><div class='line' id='LC307'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">labelWriter</span><span class="o">-&gt;</span><span class="n">SetInput</span><span class="p">(</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span> <span class="p">);</span></div><div class='line' id='LC308'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">labelWriter</span><span class="o">-&gt;</span><span class="n">SetFileName</span><span class="p">(</span> <span class="s">&quot;Integral.mhd&quot;</span><span class="p">);</span></div><div class='line' id='LC309'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">labelWriter</span><span class="o">-&gt;</span><span class="n">Write</span><span class="p">();</span></div><div class='line' id='LC310'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC311'><span class="p">}</span></div><div class='line' id='LC312'><br/></div><div class='line' id='LC313'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">setCurrLabelArray</span><span class="p">(</span><span class="n">vtkImageData</span> <span class="o">*</span><span class="n">label</span><span class="p">){</span></div><div class='line' id='LC314'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">labelVol</span><span class="o">=</span><span class="n">label</span><span class="p">;</span></div><div class='line' id='LC315'><span class="p">}</span></div><div class='line' id='LC316'><br/></div><div class='line' id='LC317'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">intializeLevelSet</span><span class="p">(){</span></div><div class='line' id='LC318'><br/></div><div class='line' id='LC319'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span> <span class="p">(</span><span class="n">Lz</span><span class="o">!=</span><span class="nb">NULL</span><span class="p">){</span></div><div class='line' id='LC320'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//destroy linked lists</span></div><div class='line' id='LC321'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lz</span><span class="p">);</span></div><div class='line' id='LC322'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Ln1</span><span class="p">);</span></div><div class='line' id='LC323'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Ln2</span><span class="p">);</span></div><div class='line' id='LC324'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lp1</span><span class="p">);</span></div><div class='line' id='LC325'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lp2</span><span class="p">);</span></div><div class='line' id='LC326'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lin2out</span><span class="p">);</span></div><div class='line' id='LC327'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lout2in</span><span class="p">);</span></div><div class='line' id='LC328'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC329'><br/></div><div class='line' id='LC330'><br/></div><div class='line' id='LC331'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//create linked lists</span></div><div class='line' id='LC332'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lz</span>  <span class="o">=</span> <span class="n">ll_create</span><span class="p">();</span></div><div class='line' id='LC333'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Ln1</span> <span class="o">=</span> <span class="n">ll_create</span><span class="p">();</span></div><div class='line' id='LC334'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Ln2</span> <span class="o">=</span> <span class="n">ll_create</span><span class="p">();</span></div><div class='line' id='LC335'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lp1</span> <span class="o">=</span> <span class="n">ll_create</span><span class="p">();</span></div><div class='line' id='LC336'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lp2</span> <span class="o">=</span> <span class="n">ll_create</span><span class="p">();</span></div><div class='line' id='LC337'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lin2out</span> <span class="o">=</span> <span class="n">ll_create</span><span class="p">();</span></div><div class='line' id='LC338'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">Lout2in</span> <span class="o">=</span> <span class="n">ll_create</span><span class="p">();</span></div><div class='line' id='LC339'><br/></div><div class='line' id='LC340'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="c1">//initialize lists, phi, and labels</span></div><div class='line' id='LC341'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ls_mask2phi3c</span><span class="p">(</span><span class="n">mask</span><span class="p">,</span><span class="n">phi</span><span class="p">,</span><span class="n">label</span><span class="p">,</span><span class="n">dims</span><span class="p">,</span><span class="n">Lz</span><span class="p">,</span><span class="n">Ln1</span><span class="p">,</span><span class="n">Ln2</span><span class="p">,</span><span class="n">Lp1</span><span class="p">,</span><span class="n">Lp2</span><span class="p">);</span></div><div class='line' id='LC342'><br/></div><div class='line' id='LC343'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span> <span class="n">currSlice</span> <span class="o">==</span> <span class="n">prevSlice</span> <span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC344'><br/></div><div class='line' id='LC345'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC346'><br/></div><div class='line' id='LC347'><span class="p">}</span></div><div class='line' id='LC348'><br/></div><div class='line' id='LC349'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">Update</span><span class="p">()</span></div><div class='line' id='LC350'><span class="p">{</span></div><div class='line' id='LC351'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">interactive_chanvese</span><span class="p">(</span><span class="n">img</span><span class="p">,</span><span class="n">phi</span><span class="p">,</span><span class="n">U_I_slice</span><span class="p">,</span><span class="n">label</span><span class="p">,</span><span class="n">dims</span><span class="p">,</span><span class="n">Lz</span><span class="p">,</span><span class="n">Ln1</span><span class="p">,</span><span class="n">Lp1</span><span class="p">,</span><span class="n">Ln2</span><span class="p">,</span><span class="n">Lp2</span><span class="p">,</span><span class="n">Lin2out</span><span class="p">,</span><span class="n">Lout2in</span><span class="p">,</span><span class="n">iter</span><span class="p">,</span><span class="n">rad</span><span class="p">,</span><span class="n">lambda</span><span class="p">,</span><span class="n">display</span><span class="p">);</span></div><div class='line' id='LC352'><br/></div><div class='line' id='LC353'>&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span><span class="n">iList</span><span class="o">!=</span><span class="nb">NULL</span><span class="p">){</span></div><div class='line' id='LC354'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span><span class="p">[]</span> <span class="n">iList</span><span class="p">;</span></div><div class='line' id='LC355'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC356'>&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span><span class="n">jList</span><span class="o">!=</span><span class="nb">NULL</span><span class="p">){</span></div><div class='line' id='LC357'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span><span class="p">[]</span> <span class="n">jList</span><span class="p">;</span></div><div class='line' id='LC358'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC359'><br/></div><div class='line' id='LC360'>&nbsp;&nbsp;<span class="c1">//get number and coordinates of point (row, col) on the zero level set</span></div><div class='line' id='LC361'>&nbsp;&nbsp;<span class="n">prep_C_output</span><span class="p">(</span><span class="n">Lz</span><span class="p">,</span><span class="n">dims</span><span class="p">,</span><span class="n">phi</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">iList</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">jList</span><span class="p">,</span> <span class="n">lengthZLS</span><span class="p">);</span></div><div class='line' id='LC362'><br/></div><div class='line' id='LC363'><br/></div><div class='line' id='LC364'>&nbsp;&nbsp;<span class="c1">//threshold phi to find segmentation label, assign it to appropriate range of label!</span></div><div class='line' id='LC365'>&nbsp;&nbsp;<span class="kt">long</span> <span class="n">elemNum</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span></div><div class='line' id='LC366'>&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">j</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span>  <span class="p">{</span></div><div class='line' id='LC367'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">i</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC368'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">double</span> <span class="n">phi_val</span> <span class="o">=</span> <span class="n">phi</span><span class="p">[</span><span class="n">elemNum</span><span class="p">];</span></div><div class='line' id='LC369'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">double</span> <span class="n">phi_out</span> <span class="o">=</span> <span class="p">(</span><span class="o">-</span><span class="n">phi_val</span> <span class="o">+</span> <span class="mf">3.0</span><span class="p">)</span> <span class="o">/</span> <span class="mf">6.0</span><span class="p">;</span> <span class="c1">// shift and scale from [-3,3] to [0,1]</span></div><div class='line' id='LC370'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ptrCurrLabel</span><span class="p">[</span><span class="n">elemNum</span><span class="p">]</span><span class="o">=</span> <span class="p">(</span><span class="kt">unsigned</span> <span class="kt">short</span><span class="p">)</span> <span class="p">(</span> <span class="p">(</span> <span class="p">(</span><span class="n">phi_out</span> <span class="o">&gt;</span> <span class="mf">0.95</span><span class="p">)</span> <span class="o">+</span> <span class="p">(</span><span class="n">phi_out</span> <span class="o">&gt;</span> <span class="mf">0.8</span><span class="p">)</span> <span class="o">+</span> <span class="p">(</span><span class="n">phi_out</span> <span class="o">&gt;</span> <span class="mf">0.65</span><span class="p">)</span> <span class="o">+</span> <span class="p">(</span><span class="n">phi_out</span> <span class="o">&gt;</span> <span class="mf">0.5</span><span class="p">)</span> <span class="p">)</span> <span class="o">*</span> <span class="n">labelRange</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="o">/</span> <span class="mf">4.0</span> <span class="p">);</span></div><div class='line' id='LC371'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">if</span><span class="p">(</span> <span class="n">phi_val</span> <span class="o">&lt;</span><span class="mi">0</span><span class="p">){</span></div><div class='line' id='LC372'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">seg</span><span class="p">[</span><span class="n">elemNum</span><span class="p">]</span><span class="o">=</span><span class="n">labelRange</span><span class="p">[</span><span class="mi">1</span><span class="p">];</span></div><div class='line' id='LC373'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span><span class="k">else</span><span class="p">{</span></div><div class='line' id='LC374'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">seg</span><span class="p">[</span><span class="n">elemNum</span><span class="p">]</span><span class="o">=</span><span class="n">labelRange</span><span class="p">[</span><span class="mi">0</span><span class="p">];</span></div><div class='line' id='LC375'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC376'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">elemNum</span><span class="o">++</span><span class="p">;</span></div><div class='line' id='LC377'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC378'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC379'>&nbsp;&nbsp;<span class="n">prevSlice</span> <span class="o">=</span> <span class="n">currSlice</span><span class="p">;</span></div><div class='line' id='LC380'><span class="p">}</span></div><div class='line' id='LC381'><br/></div><div class='line' id='LC382'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">copyIntegralDuringPaste</span><span class="p">(</span><span class="kt">int</span> <span class="n">kFrom</span><span class="p">,</span> <span class="kt">int</span> <span class="n">kTo</span><span class="p">)</span></div><div class='line' id='LC383'><span class="p">{</span></div><div class='line' id='LC384'><br/></div><div class='line' id='LC385'>&nbsp;&nbsp;<span class="n">Mat</span> <span class="n">tmp</span> <span class="o">=</span><span class="mf">0.9</span>  <span class="o">*</span> <span class="n">U_integral</span><span class="p">[</span><span class="n">kFrom</span><span class="p">];</span></div><div class='line' id='LC386'>&nbsp;&nbsp;<span class="n">tmp</span><span class="p">.</span><span class="n">copyTo</span><span class="p">(</span><span class="n">U_integral</span><span class="p">[</span><span class="n">kTo</span><span class="p">]);</span></div><div class='line' id='LC387'>&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span><span class="n">i</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">0</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span>  <span class="p">{</span></div><div class='line' id='LC388'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">for</span> <span class="p">(</span><span class="kt">int</span> <span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">j</span><span class="o">&lt;=</span><span class="n">mdims</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span><span class="o">-</span><span class="mi">1</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span></div><div class='line' id='LC389'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="kt">double</span> <span class="n">val</span><span class="o">=</span><span class="mf">0.9</span><span class="o">*</span><span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="o">-&gt;</span><span class="n">GetScalarComponentAsDouble</span><span class="p">(</span><span class="n">i</span><span class="p">,</span><span class="n">j</span><span class="p">,</span><span class="n">kFrom</span><span class="p">,</span><span class="mi">0</span><span class="p">);</span></div><div class='line' id='LC390'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">U_Integral_image</span><span class="o">-&gt;</span><span class="n">SetScalarComponentFromDouble</span><span class="p">(</span><span class="n">i</span><span class="p">,</span><span class="n">j</span><span class="p">,</span><span class="n">kTo</span><span class="p">,</span><span class="mi">0</span><span class="p">,</span><span class="n">val</span><span class="p">);</span></div><div class='line' id='LC391'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC392'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC393'><span class="p">}</span></div><div class='line' id='LC394'><br/></div><div class='line' id='LC395'><span class="kt">void</span> <span class="n">KSegmentor</span><span class="o">::</span><span class="n">setCurrIndex</span><span class="p">(</span><span class="kt">int</span> <span class="n">sliceIndex</span><span class="p">){</span></div><div class='line' id='LC396'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">currSlice</span><span class="o">=</span><span class="n">sliceIndex</span><span class="p">;</span></div><div class='line' id='LC397'><span class="p">}</span></div><div class='line' id='LC398'><br/></div><div class='line' id='LC399'><span class="n">KSegmentor</span><span class="o">::~</span><span class="n">KSegmentor</span><span class="p">(){</span></div><div class='line' id='LC400'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">mdims</span><span class="p">;</span></div><div class='line' id='LC401'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">img</span><span class="p">;</span></div><div class='line' id='LC402'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">mask</span><span class="p">;</span></div><div class='line' id='LC403'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">imgRange</span><span class="p">;</span></div><div class='line' id='LC404'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">labelRange</span><span class="p">;</span></div><div class='line' id='LC405'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">phi</span><span class="p">;</span></div><div class='line' id='LC406'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">label</span><span class="p">;</span></div><div class='line' id='LC407'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">seg</span><span class="p">;</span></div><div class='line' id='LC408'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">iList</span><span class="p">;</span></div><div class='line' id='LC409'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">jList</span><span class="p">;</span></div><div class='line' id='LC410'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">U_I_slice</span><span class="p">;</span></div><div class='line' id='LC411'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">delete</span> <span class="p">[]</span> <span class="k">this</span><span class="o">-&gt;</span><span class="n">U_t_slice</span><span class="p">;</span></div><div class='line' id='LC412'><br/></div><div class='line' id='LC413'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lz</span><span class="p">);</span></div><div class='line' id='LC414'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Ln1</span><span class="p">);</span></div><div class='line' id='LC415'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Ln2</span><span class="p">);</span></div><div class='line' id='LC416'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lp1</span><span class="p">);</span></div><div class='line' id='LC417'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lp2</span><span class="p">);</span></div><div class='line' id='LC418'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lin2out</span><span class="p">);</span></div><div class='line' id='LC419'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">ll_destroy</span><span class="p">(</span><span class="n">Lout2in</span><span class="p">);</span></div><div class='line' id='LC420'><span class="p">}</span></div><div class='line' id='LC421'><br/></div><div class='line' id='LC422'><br/></div><div class='line' id='LC423'><span class="n">KSegmentor</span><span class="o">::</span><span class="n">KSegmentor</span><span class="p">(){</span></div><div class='line' id='LC424'>&nbsp;&nbsp;<span class="k">throw</span> <span class="s">&quot;Oh snap my default constructor is bogus and doesn&#39;t allocate my stuff&quot;</span><span class="p">;</span></div><div class='line' id='LC425'><span class="p">}</span></div><div class='line' id='LC426'><br/></div><div class='line' id='LC427'><br/></div><div class='line' id='LC428'><span class="p">}</span></div><div class='line' id='LC429'><br/></div></pre></div>
          </td>
        </tr>
      </table>
  </div>

          </div>
        </div>
      </div>
    </div>

  </div>

<div class="frame frame-loading" style="display:none;" data-tree-list-url="/kfritscher/kslice/tree-list/13d71926258ea390923b4c7c12b84ab35735723e" data-blob-url-prefix="/kfritscher/kslice/blob/13d71926258ea390923b4c7c12b84ab35735723e">
  <img src="https://a248.e.akamai.net/assets.github.com/images/modules/ajax/big_spinner_336699.gif" height="32" width="32">
</div>

    </div>

    </div>

    <!-- footer -->
    <div id="footer" >
      
  <div class="upper_footer">
     <div class="site" class="clearfix">

       <!--[if IE]><h4 id="blacktocat_ie">GitHub Links</h4><![endif]-->
       <![if !IE]><h4 id="blacktocat">GitHub Links</h4><![endif]>

       <ul class="footer_nav">
         <h4>GitHub</h4>
         <li><a href="https://github.com/about">About</a></li>
         <li><a href="https://github.com/blog">Blog</a></li>
         <li><a href="https://github.com/features">Features</a></li>
         <li><a href="https://github.com/contact">Contact &amp; Support</a></li>
         <li><a href="https://github.com/training">Training</a></li>
         <li><a href="http://status.github.com/">Site Status</a></li>
       </ul>

       <ul class="footer_nav">
         <h4>Tools</h4>
         <li><a href="http://mac.github.com/">GitHub for Mac</a></li>
         <li><a href="http://mobile.github.com/">Issues for iPhone</a></li>
         <li><a href="https://gist.github.com">Gist: Code Snippets</a></li>
         <li><a href="http://enterprise.github.com/">GitHub Enterprise</a></li>
         <li><a href="http://jobs.github.com/">Job Board</a></li>
       </ul>

       <ul class="footer_nav">
         <h4>Extras</h4>
         <li><a href="http://shop.github.com/">GitHub Shop</a></li>
         <li><a href="http://octodex.github.com/">The Octodex</a></li>
       </ul>

       <ul class="footer_nav">
         <h4>Documentation</h4>
         <li><a href="http://help.github.com/">GitHub Help</a></li>
         <li><a href="http://developer.github.com/">Developer API</a></li>
         <li><a href="http://github.github.com/github-flavored-markdown/">GitHub Flavored Markdown</a></li>
         <li><a href="http://pages.github.com/">GitHub Pages</a></li>
       </ul>

     </div><!-- /.site -->
  </div><!-- /.upper_footer -->

<div class="lower_footer">
  <div class="site" class="clearfix">
    <!--[if IE]><div id="legal_ie"><![endif]-->
    <![if !IE]><div id="legal"><![endif]>
      <ul>
          <li><a href="https://github.com/site/terms">Terms of Service</a></li>
          <li><a href="https://github.com/site/privacy">Privacy</a></li>
          <li><a href="https://github.com/security">Security</a></li>
      </ul>

      <p>&copy; 2011 <span id="_rrt" title="0.26434s from fe10.rs.github.com">GitHub</span> Inc. All rights reserved.</p>
    </div><!-- /#legal or /#legal_ie-->

      <div class="sponsor">
        <a href="http://www.rackspace.com" class="logo">
          <img alt="Dedicated Server" height="36" src="https://a248.e.akamai.net/assets.github.com/images/modules/footer/rackspace_logo.png?v2" width="38" />
        </a>
        Powered by the <a href="http://www.rackspace.com ">Dedicated
        Servers</a> and<br/> <a href="http://www.rackspacecloud.com">Cloud
        Computing</a> of Rackspace Hosting<span>&reg;</span>
      </div>
  </div><!-- /.site -->
</div><!-- /.lower_footer -->

    </div><!-- /#footer -->

    

<div id="keyboard_shortcuts_pane" class="instapaper_ignore readability-extra" style="display:none">
  <h2>Keyboard Shortcuts <small><a href="#" class="js-see-all-keyboard-shortcuts">(see all)</a></small></h2>

  <div class="columns threecols">
    <div class="column first">
      <h3>Site wide shortcuts</h3>
      <dl class="keyboard-mappings">
        <dt>s</dt>
        <dd>Focus site search</dd>
      </dl>
      <dl class="keyboard-mappings">
        <dt>?</dt>
        <dd>Bring up this help dialog</dd>
      </dl>
    </div><!-- /.column.first -->

    <div class="column middle" style=&#39;display:none&#39;>
      <h3>Commit list</h3>
      <dl class="keyboard-mappings">
        <dt>j</dt>
        <dd>Move selection down</dd>
      </dl>
      <dl class="keyboard-mappings">
        <dt>k</dt>
        <dd>Move selection up</dd>
      </dl>
      <dl class="keyboard-mappings">
        <dt>c <em>or</em> o <em>or</em> enter</dt>
        <dd>Open commit</dd>
      </dl>
      <dl class="keyboard-mappings">
        <dt>y</dt>
        <dd>Expand URL to its canonical form</dd>
      </dl>
    </div><!-- /.column.first -->

    <div class="column last" style=&#39;display:none&#39;>
      <h3>Pull request list</h3>
      <dl class="keyboard-mappings">
        <dt>j</dt>
        <dd>Move selection down</dd>
      </dl>
      <dl class="keyboard-mappings">
        <dt>k</dt>
        <dd>Move selection up</dd>
      </dl>
      <dl class="keyboard-mappings">
        <dt>o <em>or</em> enter</dt>
        <dd>Open issue</dd>
      </dl>
    </div><!-- /.columns.last -->

  </div><!-- /.columns.equacols -->

  <div style=&#39;display:none&#39;>
    <div class="rule"></div>

    <h3>Issues</h3>

    <div class="columns threecols">
      <div class="column first">
        <dl class="keyboard-mappings">
          <dt>j</dt>
          <dd>Move selection down</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>k</dt>
          <dd>Move selection up</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>x</dt>
          <dd>Toggle selection</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>o <em>or</em> enter</dt>
          <dd>Open issue</dd>
        </dl>
      </div><!-- /.column.first -->
      <div class="column middle">
        <dl class="keyboard-mappings">
          <dt>I</dt>
          <dd>Mark selection as read</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>U</dt>
          <dd>Mark selection as unread</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>e</dt>
          <dd>Close selection</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>y</dt>
          <dd>Remove selection from view</dd>
        </dl>
      </div><!-- /.column.middle -->
      <div class="column last">
        <dl class="keyboard-mappings">
          <dt>c</dt>
          <dd>Create issue</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>l</dt>
          <dd>Create label</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>i</dt>
          <dd>Back to inbox</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>u</dt>
          <dd>Back to issues</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>/</dt>
          <dd>Focus issues search</dd>
        </dl>
      </div>
    </div>
  </div>

  <div style=&#39;display:none&#39;>
    <div class="rule"></div>

    <h3>Issues Dashboard</h3>

    <div class="columns threecols">
      <div class="column first">
        <dl class="keyboard-mappings">
          <dt>j</dt>
          <dd>Move selection down</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>k</dt>
          <dd>Move selection up</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>o <em>or</em> enter</dt>
          <dd>Open issue</dd>
        </dl>
      </div><!-- /.column.first -->
    </div>
  </div>

  <div style=&#39;display:none&#39;>
    <div class="rule"></div>

    <h3>Network Graph</h3>
    <div class="columns equacols">
      <div class="column first">
        <dl class="keyboard-mappings">
          <dt><span class="badmono">←</span> <em>or</em> h</dt>
          <dd>Scroll left</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt><span class="badmono">→</span> <em>or</em> l</dt>
          <dd>Scroll right</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt><span class="badmono">↑</span> <em>or</em> k</dt>
          <dd>Scroll up</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt><span class="badmono">↓</span> <em>or</em> j</dt>
          <dd>Scroll down</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>t</dt>
          <dd>Toggle visibility of head labels</dd>
        </dl>
      </div><!-- /.column.first -->
      <div class="column last">
        <dl class="keyboard-mappings">
          <dt>shift <span class="badmono">←</span> <em>or</em> shift h</dt>
          <dd>Scroll all the way left</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>shift <span class="badmono">→</span> <em>or</em> shift l</dt>
          <dd>Scroll all the way right</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>shift <span class="badmono">↑</span> <em>or</em> shift k</dt>
          <dd>Scroll all the way up</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>shift <span class="badmono">↓</span> <em>or</em> shift j</dt>
          <dd>Scroll all the way down</dd>
        </dl>
      </div><!-- /.column.last -->
    </div>
  </div>

  <div >
    <div class="rule"></div>
    <div class="columns threecols">
      <div class="column first" >
        <h3>Source Code Browsing</h3>
        <dl class="keyboard-mappings">
          <dt>t</dt>
          <dd>Activates the file finder</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>l</dt>
          <dd>Jump to line</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>w</dt>
          <dd>Switch branch/tag</dd>
        </dl>
        <dl class="keyboard-mappings">
          <dt>y</dt>
          <dd>Expand URL to its canonical form</dd>
        </dl>
      </div>
    </div>
  </div>
</div>

    <div id="markdown-help" class="instapaper_ignore readability-extra">
  <h2>Markdown Cheat Sheet</h2>

  <div class="cheatsheet-content">

  <div class="mod">
    <div class="col">
      <h3>Format Text</h3>
      <p>Headers</p>
      <pre>
# This is an &lt;h1&gt; tag
## This is an &lt;h2&gt; tag
###### This is an &lt;h6&gt; tag</pre>
     <p>Text styles</p>
     <pre>
*This text will be italic*
_This will also be italic_
**This text will be bold**
__This will also be bold__

*You **can** combine them*
</pre>
    </div>
    <div class="col">
      <h3>Lists</h3>
      <p>Unordered</p>
      <pre>
* Item 1
* Item 2
  * Item 2a
  * Item 2b</pre>
     <p>Ordered</p>
     <pre>
1. Item 1
2. Item 2
3. Item 3
   * Item 3a
   * Item 3b</pre>
    </div>
    <div class="col">
      <h3>Miscellaneous</h3>
      <p>Images</p>
      <pre>
![GitHub Logo](/images/logo.png)
Format: ![Alt Text](url)
</pre>
     <p>Links</p>
     <pre>
http://github.com - automatic!
[GitHub](http://github.com)</pre>
<p>Blockquotes</p>
     <pre>
As Kanye West said:
> We're living the future so
> the present is our past.
</pre>
    </div>
  </div>
  <div class="rule"></div>

  <h3>Code Examples in Markdown</h3>
  <div class="col">
      <p>Syntax highlighting with <a href="http://github.github.com/github-flavored-markdown/" title="GitHub Flavored Markdown" target="_blank">GFM</a></p>
      <pre>
```javascript
function fancyAlert(arg) {
  if(arg) {
    $.facebox({div:'#foo'})
  }
}
```</pre>
    </div>
    <div class="col">
      <p>Or, indent your code 4 spaces</p>
      <pre>
Here is a Python code example
without syntax highlighting:

    def foo:
      if not bar:
        return true</pre>
    </div>
    <div class="col">
      <p>Inline code for comments</p>
      <pre>
I think you should use an
`&lt;addr&gt;` element here instead.</pre>
    </div>
  </div>

  </div>
</div>

    <div class="context-overlay"></div>

    <div class="ajax-error-message">
      <p><span class="icon"></span> Something went wrong with that request. Please try again. <a href="javascript:;" class="ajax-error-dismiss">Dismiss</a></p>
    </div>

    
    
    
  </body>
</html>

