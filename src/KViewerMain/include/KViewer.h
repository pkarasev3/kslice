


<!DOCTYPE html>
<html>
  <head>
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="chrome=1">
        <title>src/KViewerMain/include/KViewer.h at KSlice3D from kfritscher/kslice - GitHub</title>
    <link rel="search" type="application/opensearchdescription+xml" href="/opensearch.xml" title="GitHub" />
    <link rel="fluid-icon" href="https://github.com/fluidicon.png" title="GitHub" />

    
    

    <meta content="authenticity_token" name="csrf-param" />
<meta content="c053128708b2ec4dce14d754e886b2abb2a6c722" name="csrf-token" />

    <link href="https://a248.e.akamai.net/assets.github.com/stylesheets/bundles/github-9bef515c140b863f61df966f44e62caaa19ccae1.css" media="screen" rel="stylesheet" type="text/css" />
    

    <script src="https://a248.e.akamai.net/assets.github.com/javascripts/bundles/jquery-0afaa9d8025004af7fc6f2a561837057d3f21298.js" type="text/javascript"></script>
    <script src="https://a248.e.akamai.net/assets.github.com/javascripts/bundles/github-c3520f89f16a33cc97a93889496732ddc0d6c49b.js" type="text/javascript"></script>
    

      <link rel='permalink' href='/kfritscher/kslice/blob/13d71926258ea390923b4c7c12b84ab35735723e/src/KViewerMain/include/KViewer.h'>
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
      <li class="login"><a href="https://github.com/login?return_to=%2Fkfritscher%2Fkslice%2Fblob%2FKSlice3D%2Fsrc%2FKViewerMain%2Finclude%2FKViewer.h">Login</a></li>
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
                      <a href="/kfritscher/kslice/blob/KSlice3D/src/KViewerMain/include/KViewer.h" data-name="KSlice3D">KSlice3D</a>
                  </h4>
                </div>
                <div class="commitish-item branch-commitish selector-item">
                  <h4>
                      <a href="/kfritscher/kslice/blob/master/src/KViewerMain/include/KViewer.h" data-name="master">master</a>
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

    <div class="breadcrumb" data-path="src/KViewerMain/include/KViewer.h/">
      <b><a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e" class="js-rewrite-sha">kslice</a></b> / <a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e/src" class="js-rewrite-sha">src</a> / <a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e/src/KViewerMain" class="js-rewrite-sha">KViewerMain</a> / <a href="/kfritscher/kslice/tree/13d71926258ea390923b4c7c12b84ab35735723e/src/KViewerMain/include" class="js-rewrite-sha">include</a> / KViewer.h       <span style="display:none" id="clippy_3236" class="clippy-text">src/KViewerMain/include/KViewer.h</span>
      
      <object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000"
              width="110"
              height="14"
              class="clippy"
              id="clippy" >
      <param name="movie" value="https://a248.e.akamai.net/assets.github.com/flash/clippy.swf?v5"/>
      <param name="allowScriptAccess" value="always" />
      <param name="quality" value="high" />
      <param name="scale" value="noscale" />
      <param NAME="FlashVars" value="id=clippy_3236&amp;copied=copied!&amp;copyto=copy to clipboard">
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
             FlashVars="id=clippy_3236&amp;copied=copied!&amp;copyto=copy to clipboard"
             bgcolor="#FFFFFF"
             wmode="opaque"
      />
      </object>
      

    </div>

    <div class="frames">
      <div class="frame frame-center" data-path="src/KViewerMain/include/KViewer.h/" data-permalink-url="/kfritscher/kslice/blob/13d71926258ea390923b4c7c12b84ab35735723e/src/KViewerMain/include/KViewer.h" data-title="src/KViewerMain/include/KViewer.h at KSlice3D from kfritscher/kslice - GitHub" data-type="blob">
          <ul class="big-actions">
            <li><a class="file-edit-link minibutton js-rewrite-sha" href="/kfritscher/kslice/edit/13d71926258ea390923b4c7c12b84ab35735723e/src/KViewerMain/include/KViewer.h" data-method="post"><span>Edit this file</span></a></li>
          </ul>

        <div id="files">
          <div class="file">
            <div class="meta">
              <div class="info">
                <span class="icon"><img alt="Txt" height="16" src="https://a248.e.akamai.net/assets.github.com/images/icons/txt.png" width="16" /></span>
                <span class="mode" title="File Mode">100644</span>
                  <span>224 lines (164 sloc)</span>
                <span>6.62 kb</span>
              </div>
              <ul class="actions">
                <li><a href="/kfritscher/kslice/raw/KSlice3D/src/KViewerMain/include/KViewer.h" id="raw-url">raw</a></li>
                  <li><a href="/kfritscher/kslice/blame/KSlice3D/src/KViewerMain/include/KViewer.h">blame</a></li>
                <li><a href="/kfritscher/kslice/commits/KSlice3D/src/KViewerMain/include/KViewer.h">history</a></li>
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
</pre>
          </td>
          <td width="100%">
                <div class="highlight"><pre><div class='line' id='LC1'><span class="cm">/*=========================================================================</span></div><div class='line' id='LC2'><br/></div><div class='line' id='LC3'><span class="cm">  Copyright 2004 Sandia Corporation.</span></div><div class='line' id='LC4'><span class="cm">  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive</span></div><div class='line' id='LC5'><span class="cm">  license for use of this work by or on behalf of the</span></div><div class='line' id='LC6'><span class="cm">  U.S. Government. Redistribution and use in source and binary forms, with</span></div><div class='line' id='LC7'><span class="cm">  or without modification, are permitted provided that this Notice and any</span></div><div class='line' id='LC8'><span class="cm">  statement of authorship are reproduced on all copies.</span></div><div class='line' id='LC9'><br/></div><div class='line' id='LC10'><span class="cm">=========================================================================*/</span></div><div class='line' id='LC11'><br/></div><div class='line' id='LC12'><span class="cm">/*========================================================================</span></div><div class='line' id='LC13'><span class="cm"> For general information about using VTK and Qt, see:</span></div><div class='line' id='LC14'><span class="cm"> http://www.trolltech.com/products/3rdparty/vtksupport.html</span></div><div class='line' id='LC15'><span class="cm">=========================================================================*/</span></div><div class='line' id='LC16'><br/></div><div class='line' id='LC17'><span class="cm">/*========================================================================</span></div><div class='line' id='LC18'><span class="cm"> !!! WARNING for those who want to contribute code to this file.</span></div><div class='line' id='LC19'><span class="cm"> !!! If you use a commercial edition of Qt, you can modify this code.</span></div><div class='line' id='LC20'><span class="cm"> !!! If you use an open source version of Qt, you are free to modify</span></div><div class='line' id='LC21'><span class="cm"> !!! and use this code within the guidelines of the GPL license.</span></div><div class='line' id='LC22'><span class="cm"> !!! Unfortunately, you cannot contribute the changes back into this</span></div><div class='line' id='LC23'><span class="cm"> !!! file.  Doing so creates a conflict between the GPL and BSD-like VTK</span></div><div class='line' id='LC24'><span class="cm"> !!! license.</span></div><div class='line' id='LC25'><span class="cm">=========================================================================*/</span></div><div class='line' id='LC26'><br/></div><div class='line' id='LC27'><span class="cp">#ifndef KVIEWER_GUI_H</span></div><div class='line' id='LC28'><span class="cp">#define KVIEWER_GUI_H</span></div><div class='line' id='LC29'><br/></div><div class='line' id='LC30'><span class="cp">#include  &lt;QMainWindow&gt;</span></div><div class='line' id='LC31'><span class="cp">#include  &lt;vtkSmartPointer.h&gt;</span></div><div class='line' id='LC32'><span class="cp">#include  &quot;GUISetup.h&quot;</span></div><div class='line' id='LC33'><span class="cp">#include  &quot;KWidget_2D_left.h&quot;</span></div><div class='line' id='LC34'><span class="cp">#include  &quot;KWidget_3D_right.h&quot;</span></div><div class='line' id='LC35'><span class="cp">#include &quot;vtkRegularPolygonSource.h&quot;</span></div><div class='line' id='LC36'><span class="cp">#include &quot;vtkProperty.h&quot;</span></div><div class='line' id='LC37'><br/></div><div class='line' id='LC38'><span class="k">class</span> <span class="nc">vtkImageThreshold</span><span class="p">;</span></div><div class='line' id='LC39'><span class="k">class</span> <span class="nc">vtkRenderer</span><span class="p">;</span></div><div class='line' id='LC40'><span class="k">class</span> <span class="nc">vtkEventQtSlotConnect</span><span class="p">;</span></div><div class='line' id='LC41'><span class="k">class</span> <span class="nc">vtkObject</span><span class="p">;</span></div><div class='line' id='LC42'><span class="k">class</span> <span class="nc">vtkCommand</span><span class="p">;</span></div><div class='line' id='LC43'><span class="k">class</span> <span class="nc">KvtkImageInteractionCallback</span><span class="p">;</span></div><div class='line' id='LC44'><br/></div><div class='line' id='LC45'><span class="k">using</span> <span class="n">cv</span><span class="o">::</span><span class="n">Ptr</span><span class="p">;</span></div><div class='line' id='LC46'><br/></div><div class='line' id='LC47'><span class="k">class</span> <span class="nc">KViewer</span> <span class="o">:</span> <span class="k">public</span> <span class="n">QMainWindow</span> <span class="p">,</span> <span class="k">public</span> <span class="n">Ui</span><span class="o">::</span><span class="n">GUI</span></div><div class='line' id='LC48'><span class="p">{</span></div><div class='line' id='LC49'>&nbsp;&nbsp;<span class="n">Q_OBJECT</span></div><div class='line' id='LC50'><br/></div><div class='line' id='LC51'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkPolyDataMapper</span><span class="o">&gt;</span> <span class="n">m_CircleMapper</span><span class="p">;</span></div><div class='line' id='LC52'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkActor</span><span class="o">&gt;</span> <span class="n">m_CircleActor</span><span class="p">;</span></div><div class='line' id='LC53'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkRegularPolygonSource</span><span class="o">&gt;</span> <span class="n">m_Circle</span><span class="p">;</span></div><div class='line' id='LC54'><br/></div><div class='line' id='LC55'><span class="k">public</span><span class="o">:</span></div><div class='line' id='LC56'>&nbsp;&nbsp;<span class="n">KViewer</span><span class="p">(</span> <span class="p">)</span> <span class="p">{</span> <span class="p">}</span></div><div class='line' id='LC57'>&nbsp;&nbsp;<span class="n">KViewer</span><span class="p">(</span> <span class="k">const</span> <span class="n">KViewerOptions</span><span class="o">&amp;</span> <span class="n">input_options</span><span class="p">);</span></div><div class='line' id='LC58'>&nbsp;&nbsp;<span class="k">virtual</span> <span class="o">~</span><span class="n">KViewer</span><span class="p">();</span></div><div class='line' id='LC59'>&nbsp;&nbsp;<span class="kt">int</span> <span class="n">round</span><span class="p">(</span><span class="kt">double</span> <span class="n">a</span><span class="p">);</span></div><div class='line' id='LC60'><br/></div><div class='line' id='LC61'><span class="k">public</span> <span class="nl">slots:</span></div><div class='line' id='LC62'><br/></div><div class='line' id='LC63'>&nbsp;&nbsp;<span class="cm">/**  \brief display the mouse coords in main QT window</span></div><div class='line' id='LC64'><span class="cm">    */</span></div><div class='line' id='LC65'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">updateCoords</span><span class="p">(</span><span class="n">vtkObject</span><span class="o">*</span><span class="p">);</span></div><div class='line' id='LC66'><br/></div><div class='line' id='LC67'>&nbsp;&nbsp;<span class="cm">/**  \brief Handle slider update and show new slice in QVTK window</span></div><div class='line' id='LC68'><span class="cm">    */</span></div><div class='line' id='LC69'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">SliceSelect</span><span class="p">(</span><span class="kt">int</span> <span class="n">SliderVal</span><span class="p">);</span></div><div class='line' id='LC70'><br/></div><div class='line' id='LC71'>&nbsp;&nbsp;<span class="cm">/**  \brief Callback for &#39;click on toggle draw/erase button&#39;</span></div><div class='line' id='LC72'><span class="cm">    */</span></div><div class='line' id='LC73'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">ToggleFillEraseMode</span><span class="p">();</span></div><div class='line' id='LC74'><br/></div><div class='line' id='LC75'>&nbsp;&nbsp;<span class="cm">/**  \brief Callback for &#39;click on update model button&#39;</span></div><div class='line' id='LC76'><span class="cm">    */</span></div><div class='line' id='LC77'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">UpdateModel3D</span><span class="p">();</span></div><div class='line' id='LC78'>&nbsp;&nbsp;</div><div class='line' id='LC79'><br/></div><div class='line' id='LC80'>&nbsp;&nbsp;<span class="cm">/** \brief Record to file the current label map segmentation result</span></div><div class='line' id='LC81'><span class="cm">    */</span></div><div class='line' id='LC82'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">SaveSegmentation</span><span class="p">(</span> <span class="p">);</span></div><div class='line' id='LC83'><br/></div><div class='line' id='LC84'>&nbsp;&nbsp;<span class="cm">/** Saves as user chosen filename</span></div><div class='line' id='LC85'><span class="cm">    */</span></div><div class='line' id='LC86'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">SaveAsSegmentation</span><span class="p">();</span></div><div class='line' id='LC87'><br/></div><div class='line' id='LC88'>&nbsp;&nbsp;<span class="cm">/**  \brief handle click to paint event in draw window</span></div><div class='line' id='LC89'><span class="cm">    */</span></div><div class='line' id='LC90'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">mousePaintEvent</span><span class="p">(</span><span class="n">vtkObject</span><span class="o">*</span> <span class="n">obj</span><span class="p">);</span></div><div class='line' id='LC91'><br/></div><div class='line' id='LC92'>&nbsp;&nbsp;<span class="cm">/**  \brief File -&gt; Load new image file (.mha)</span></div><div class='line' id='LC93'><span class="cm">    */</span></div><div class='line' id='LC94'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">LoadImage</span><span class="p">();</span></div><div class='line' id='LC95'><br/></div><div class='line' id='LC96'>&nbsp;&nbsp;<span class="cm">/**  \brief File -&gt; Load new label file (.mha)</span></div><div class='line' id='LC97'><span class="cm">    */</span></div><div class='line' id='LC98'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">LoadLabelMap</span><span class="p">();</span></div><div class='line' id='LC99'><br/></div><div class='line' id='LC100'>&nbsp;&nbsp;<span class="cm">/** \brief Help -&gt; About</span></div><div class='line' id='LC101'><span class="cm">  */</span></div><div class='line' id='LC102'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">About</span><span class="p">();</span></div><div class='line' id='LC103'><br/></div><div class='line' id='LC104'>&nbsp;&nbsp;<span class="cm">/** \brief update the labels showing brush size and on/off</span></div><div class='line' id='LC105'><span class="cm">    */</span></div><div class='line' id='LC106'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">updatePaintBrushStatus</span><span class="p">(</span><span class="n">vtkObject</span><span class="o">*</span><span class="p">);</span></div><div class='line' id='LC107'><br/></div><div class='line' id='LC108'>&nbsp;&nbsp;<span class="cm">/**  \brief update the volume count and display it in qlabel</span></div><div class='line' id='LC109'><span class="cm">  */</span></div><div class='line' id='LC110'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">UpdateVolumeStatus</span><span class="p">();</span></div><div class='line' id='LC111'><br/></div><div class='line' id='LC112'>&nbsp;&nbsp;<span class="cm">/**  \brief refresh display of all labels, e.g. if opacity or color are changed via gui</span></div><div class='line' id='LC113'><span class="cm">  */</span></div><div class='line' id='LC114'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">UpdateMultiLabelDisplay</span><span class="p">();</span></div><div class='line' id='LC115'><br/></div><div class='line' id='LC116'>&nbsp;&nbsp;<span class="cm">/**  \brief update the volume count and display it in qlabel</span></div><div class='line' id='LC117'><span class="cm">   */</span></div><div class='line' id='LC118'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">AddNewLabelMap</span><span class="p">(</span> <span class="p">);</span></div><div class='line' id='LC119'><br/></div><div class='line' id='LC120'><br/></div><div class='line' id='LC121'>&nbsp;&nbsp;<span class="cm">/** return the index, stored in left widget</span></div><div class='line' id='LC122'><span class="cm">    */</span></div><div class='line' id='LC123'>&nbsp;&nbsp;<span class="kt">int</span>  <span class="n">getCurrentSliceIndex</span><span class="p">()</span> <span class="p">{</span></div><div class='line' id='LC124'>&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">return</span> <span class="n">kwidget_2d_left</span><span class="o">-&gt;</span><span class="n">currentSliceIndex</span><span class="p">;</span></div><div class='line' id='LC125'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC126'>&nbsp;&nbsp;</div><div class='line' id='LC127'>&nbsp;&nbsp;<span class="cm">/** don&#39;t try to do it yet, this is called from a pipelined function */</span></div><div class='line' id='LC128'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">queueCopyFromCurrentSlice</span><span class="p">(</span> <span class="kt">int</span> <span class="n">idxFrom</span> <span class="o">=</span> <span class="o">-</span><span class="mi">1</span> <span class="p">);</span></div><div class='line' id='LC129'>&nbsp;&nbsp;</div><div class='line' id='LC130'>&nbsp;&nbsp;<span class="cm">/** load the single-slice sfls contour segmentation, updates kvdata in place */</span></div><div class='line' id='LC131'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">executeSingleSliceSegmentation</span><span class="p">(</span> <span class="p">);</span></div><div class='line' id='LC132'>&nbsp;&nbsp;</div><div class='line' id='LC133'>&nbsp;&nbsp;<span class="cm">/** now do the copy operation, stuff should be in-sync now */</span></div><div class='line' id='LC134'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">handleGenericEvent</span><span class="p">(</span> <span class="n">vtkObject</span><span class="o">*</span> <span class="n">obj</span><span class="p">,</span> <span class="kt">unsigned</span> <span class="kt">long</span> <span class="n">event</span><span class="p">);</span></div><div class='line' id='LC135'><br/></div><div class='line' id='LC136'>&nbsp;&nbsp;<span class="cm">/** Button-Press version of queueCopyFromCurrentSlice */</span></div><div class='line' id='LC137'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">copyFromCurrentSlice</span><span class="p">(</span> <span class="p">);</span></div><div class='line' id='LC138'>&nbsp;&nbsp;</div><div class='line' id='LC139'>&nbsp;&nbsp;<span class="cm">/** Button-Press version of handleGenericEvent( ... ) */</span></div><div class='line' id='LC140'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">pasteToCurrentSlice</span><span class="p">(</span> <span class="p">);</span></div><div class='line' id='LC141'><br/></div><div class='line' id='LC142'>&nbsp;&nbsp;<span class="cm">/** move the slider and update everything by +/- amount */</span></div><div class='line' id='LC143'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">MoveSlider</span><span class="p">(</span> <span class="kt">int</span> <span class="n">shiftNumberOfSlices</span> <span class="o">=</span> <span class="mi">0</span> <span class="p">);</span></div><div class='line' id='LC144'><br/></div><div class='line' id='LC145'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">SetCircleCursorSize</span><span class="p">(</span><span class="kt">unsigned</span> <span class="kt">int</span> <span class="n">size</span><span class="p">){</span></div><div class='line' id='LC146'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_Circle</span><span class="o">-&gt;</span><span class="n">SetRadius</span><span class="p">(</span><span class="n">size</span><span class="p">);</span></div><div class='line' id='LC147'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC148'><br/></div><div class='line' id='LC149'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">SetCircleCursorOpacity</span><span class="p">(</span><span class="kt">float</span> <span class="n">opacity</span><span class="p">)</span></div><div class='line' id='LC150'>&nbsp;&nbsp;<span class="p">{</span></div><div class='line' id='LC151'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="k">this</span><span class="o">-&gt;</span><span class="n">m_CircleActor</span><span class="o">-&gt;</span><span class="n">GetProperty</span><span class="p">()</span><span class="o">-&gt;</span><span class="n">SetOpacity</span><span class="p">(</span><span class="n">opacity</span><span class="p">);</span></div><div class='line' id='LC152'>&nbsp;&nbsp;<span class="p">}</span></div><div class='line' id='LC153'><br/></div><div class='line' id='LC154'><br/></div><div class='line' id='LC155'><span class="k">protected</span><span class="o">:</span></div><div class='line' id='LC156'>&nbsp;&nbsp;<span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">vtkEventQtSlotConnect</span><span class="o">&gt;</span>  <span class="n">Connections</span><span class="p">;</span></div><div class='line' id='LC157'>&nbsp;&nbsp;<span class="n">vtkSmartPointer</span><span class="o">&lt;</span><span class="n">KvtkImageInteractionCallback</span><span class="o">&gt;</span> <span class="n">image_callback</span><span class="p">;</span></div><div class='line' id='LC158'>&nbsp;&nbsp;<span class="c1">// Stores some input and state parameters that don&#39;t fit neatly elsewhere.</span></div><div class='line' id='LC159'>&nbsp;&nbsp;<span class="n">Ptr</span><span class="o">&lt;</span><span class="n">KViewerOptions</span><span class="o">&gt;</span>  <span class="n">kv_opts</span><span class="p">;</span></div><div class='line' id='LC160'>&nbsp;&nbsp;<span class="n">Ptr</span><span class="o">&lt;</span><span class="n">KDataWarehouse</span><span class="o">&gt;</span>  <span class="n">kv_data</span><span class="p">;</span></div><div class='line' id='LC161'><br/></div><div class='line' id='LC162'>&nbsp;&nbsp;<span class="c1">// top-level state containers and interaction handlers</span></div><div class='line' id='LC163'>&nbsp;&nbsp;<span class="n">Ptr</span><span class="o">&lt;</span><span class="n">KWidget_2D_left</span><span class="o">&gt;</span>   <span class="n">kwidget_2d_left</span><span class="p">;</span></div><div class='line' id='LC164'>&nbsp;&nbsp;<span class="n">Ptr</span><span class="o">&lt;</span><span class="n">KWidget_3D_right</span><span class="o">&gt;</span>  <span class="n">kwidget_3d_right</span><span class="p">;</span></div><div class='line' id='LC165'><br/></div><div class='line' id='LC166'>&nbsp;&nbsp;<span class="n">clock_t</span> <span class="n">t1</span><span class="p">,</span> <span class="n">t2</span><span class="p">;</span></div><div class='line' id='LC167'><br/></div><div class='line' id='LC168'>&nbsp;&nbsp;<span class="cm">/** \brief Creates circle cursor</span></div><div class='line' id='LC169'><span class="cm">    */</span></div><div class='line' id='LC170'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">InitializeCircleCursor</span><span class="p">();</span></div><div class='line' id='LC171'><br/></div><div class='line' id='LC172'><br/></div><div class='line' id='LC173'>&nbsp;&nbsp;<span class="cm">/** \brief Setup slot/signal connections for GUI to fire the member functions</span></div><div class='line' id='LC174'><span class="cm">    */</span></div><div class='line' id='LC175'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">ConnectQTSlots</span><span class="p">(</span> <span class="p">);</span></div><div class='line' id='LC176'><br/></div><div class='line' id='LC177'>&nbsp;&nbsp;<span class="cm">/** \brief load the data files and setup QVTK widgets, setup dimensions for</span></div><div class='line' id='LC178'><span class="cm">    *  LUT, slider based on the data file read. Called on startup and file loading.</span></div><div class='line' id='LC179'><span class="cm">    */</span></div><div class='line' id='LC180'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">setupQVTKandData</span><span class="p">(</span> <span class="p">);</span></div><div class='line' id='LC181'><br/></div><div class='line' id='LC182'>&nbsp;&nbsp;<span class="cm">/** \brief Update image information after transformation</span></div><div class='line' id='LC183'><span class="cm">    */</span></div><div class='line' id='LC184'>&nbsp;&nbsp;<span class="kt">void</span> <span class="n">UpdateImageInformation</span><span class="p">(</span><span class="n">vtkImageData</span><span class="o">*</span> <span class="n">image</span><span class="p">);</span></div><div class='line' id='LC185'><br/></div><div class='line' id='LC186'><span class="p">};</span></div><div class='line' id='LC187'><br/></div><div class='line' id='LC188'><span class="cp">#endif </span><span class="c1">// KVIEWER_GUI_h</span></div><div class='line' id='LC189'><br/></div><div class='line' id='LC190'><br/></div><div class='line' id='LC191'><span class="cp">// Getting it to build in Linux or Mac: you must follow guidelines noted here:</span></div><div class='line' id='LC192'><span class="cp">// http://gcc.gnu.org/faq.html#vtables</span></div><div class='line' id='LC193'><span class="cp">// Namely, since you inherit from QMainWindow and Ui::GUI, you have to provide</span></div><div class='line' id='LC194'><span class="cp">// bodies for all pure virtual functions declared in those base classes.</span></div><div class='line' id='LC195'><span class="cp">// Microsoft C++ compiler circumvents this in a weird way, but it is the kosher c++ spec.</span></div><div class='line' id='LC196'><span class="cp">// so, if there&#39;s a method &quot;virtual void foo()&quot; in QMainWindow, you could write</span></div><div class='line' id='LC197'><span class="cp">// void KViewer::foo() { /* null */ }</span></div><div class='line' id='LC198'><span class="cp">// to handle it.</span></div><div class='line' id='LC199'><br/></div><div class='line' id='LC200'><br/></div><div class='line' id='LC201'><span class="cp">#if 0</span><span class="c">   // gulag</span></div><div class='line' id='LC202'><br/></div><div class='line' id='LC203'><span class="c">  /** \brief Create thresholding operator during startup, connect it to UI interactors</span></div><div class='line' id='LC204'><span class="c">    */</span></div><div class='line' id='LC205'><span class="c">  void CreateThresholdFilter( );</span></div><div class='line' id='LC206'><br/></div><div class='line' id='LC207'><span class="c">  /**  \brief Callback for update min threshold slider</span></div><div class='line' id='LC208'><span class="c">    */</span></div><div class='line' id='LC209'><span class="c">  void minThreshSliderChanged(int lowerLimit);</span></div><div class='line' id='LC210'><br/></div><div class='line' id='LC211'><span class="c">  /**  \brief Callback for update max threshold slider</span></div><div class='line' id='LC212'><span class="c">    */</span></div><div class='line' id='LC213'><span class="c">  void maxThreshSliderChanged(int upperLimit);</span></div><div class='line' id='LC214'><br/></div><div class='line' id='LC215'><span class="c">  /////////////////////////// Move to KSegmentor, or a new KWidget?</span></div><div class='line' id='LC216'><span class="c">      vtkSmartPointer&lt;vtkImageActor&gt;          threshActor2D;</span></div><div class='line' id='LC217'><span class="c">      vtkSmartPointer&lt;vtkImageThreshold&gt;      thresholdFilt;</span></div><div class='line' id='LC218'><span class="c">      vtkSmartPointer&lt;vtkImageReslice&gt;        threshReslicer;</span></div><div class='line' id='LC219'><span class="c"> ////////////////////////</span></div><div class='line' id='LC220'><br/></div><div class='line' id='LC221'><br/></div><div class='line' id='LC222'><br/></div><div class='line' id='LC223'><span class="cp">#endif</span></div><div class='line' id='LC224'><br/></div></pre></div>
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

      <p>&copy; 2011 <span id="_rrt" title="0.38890s from fe12.rs.github.com">GitHub</span> Inc. All rights reserved.</p>
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

